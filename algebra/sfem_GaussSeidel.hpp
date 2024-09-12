#ifndef SFEM_GUASS_SEIDEL_HPP
#define SFEM_GUASS_SEIDEL_HPP

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

#include "sfem_MatrixFreeLinearSolver.hpp"
#include "sfem_crs_SpMV.hpp"

namespace sfem {
    template <typename T>
    class Smoother final : public MatrixFreeLinearSolver<T> {
    public:
        // Operator
        std::function<void(const T* const, T* const)> apply_op;
        std::function<void(const T* const, T* const)> left_preconditioner_op;
        std::function<void(const T* const, T* const)> right_preconditioner_op;

        // Mem management
        std::function<T*(const std::size_t)> allocate;
        std::function<void(T*)> destroy;

        std::function<void(const std::size_t, T* const x)> zeros;

        std::function<void(const ptrdiff_t, const T* const, T* const)> copy;

        // blas
        std::function<T(const ptrdiff_t, const T* const, const T* const)> dot;
        std::function<void(const ptrdiff_t, const T, const T* const, const T, T* const)> axpby;
        std::function<
                void(const ptrdiff_t, const T, const T* const, const T, const T* const, T* const)>
                zaxpby;

        // x[i] += r[i] / d[i];
        std::function<void(const std::size_t, const T* const, T* const)> smooth_;

        ptrdiff_t n_dofs{-1};

        inline std::ptrdiff_t rows() const override { return n_dofs; }
        inline std::ptrdiff_t cols() const override { return n_dofs; }

        void set_op(const std::shared_ptr<Operator<T>>& op) override {
            set_n_dofs(op->rows());
            this->apply_op = [=](const T* const x, T* const y) { op->apply(x, y); };
        }

        void set_preconditioner_op(const std::shared_ptr<Operator<T>>& op) override {
            assert(false);
        }

        void set_max_it(const int it) override { max_it = it; }

        void set_preconditioner(std::function<void(const T* const, T* const)>&& in) {
            // left_preconditioner_op = in;
            right_preconditioner_op = in;
        }

        // Solver parameters
        T tol{1e-10};
        int max_it{10000};
        int check_each{100};
        bool verbose{false};

        ExecutionSpace execution_space_{EXECUTION_SPACE_INVALID};

        ExecutionSpace execution_space() const override { return execution_space_; }

        void default_init() {
            allocate = [](const ptrdiff_t n) -> T* { return (T*)calloc(n, sizeof(T)); };

            destroy = [](T* a) { free(a); };

            copy = [](const ptrdiff_t n, const T* const src, T* const dest) {
                std::memcpy(dest, src, n * sizeof(T));
            };

            dot = [](const ptrdiff_t n, const T* const l, const T* const r) -> T {
                T ret = 0;

#pragma omp parallel for reduction(+ : ret)
                for (ptrdiff_t i = 0; i < n; i++) {
                    ret += l[i] * r[i];
                }

                return ret;
            };

            axpby = [](const ptrdiff_t n,
                       const T alpha,
                       const T* const x,
                       const T beta,
                       T* const y) {
#pragma omp parallel for
                for (ptrdiff_t i = 0; i < n; i++) {
                    y[i] = alpha * x[i] + beta * y[i];
                }
            };

            zaxpby = [](const ptrdiff_t n,
                        const T alpha,
                        const T* const x,
                        const T beta,
                        const T* const y,
                        T* const z) {
#pragma omp parallel for
                for (ptrdiff_t i = 0; i < n; i++) {
                    z[i] = alpha * x[i] + beta * y[i];
                }
            };

            zeros = [](const std::size_t n, T* const x) { memset(x, 0, n * sizeof(T)); };

            execution_space_ = EXECUTION_SPACE_HOST;
        }

        bool good() const {
            assert(allocate);
            assert(destroy);
            assert(copy);
            assert(dot);
            assert(axpby);
            assert(zaxpby);
            assert(apply_op);
            assert(smooth_);

            return allocate && destroy && copy && dot && axpby && zaxpby && apply_op && smooth_;
        }

        void monitor(const int iter, const T residual) {
            if (verbose) {
                std::cout << iter << ": " << residual << "\n";
            }
        }

        int apply(const ptrdiff_t n, const T* const b, T* const x) {
            if (!good()) {
                return -1;
            }

            T* r = allocate(n);

            // Residual
            apply_op(x, r);
            axpby(n, 1, b, -1, r);

            const T norm_r0 = dot(n, r, r);
            T norm_r = norm_r0;
            if (sqrt(norm_r) < tol) {
                destroy(r);
                return 0;
            }

            int info = -1;
            int k = 1;
            for (; k < max_it; k++) {
                smooth_(n, b, x);

                if (k % check_each == 0) {  
                    zeros(n, r);
                    apply_op(x, r);
                    axpby(n, 1, b, -1, r);
                    const T norm_r = sqrt(dot(n, r, r));
                    monitor(k, norm_r);

                    if (norm_r < tol || norm_r != norm_r) {
                        assert(norm_r == norm_r);
                        break;
                    }
                }
            }

            if (verbose) {
                const T norm_r = sqrt(dot(n, r, r));
                std::printf("Finished at iteration %d with |r| = %g, reduction %g\n",
                            k,
                            (double)norm_r,
                            (double)(norm_r / norm_r0));
            }

            // clean-up
            destroy(r);
            return info;
        }

        int apply(const T* const b, T* const x) override { return apply(n_dofs, b, x); }
        void set_n_dofs(const ptrdiff_t n) override { this->n_dofs = n; }
    };

    template <typename R,  typename C, typename T>
    std::shared_ptr<Smoother<T>> h_gauss_seidel(
        const std::shared_ptr<CRSSpMV<R, C, T>> &crs,
        const T*d
        )
    {
        auto gs = std::make_shared<Smoother<T>>();
        gs->set_op(crs);

        gs->smooth_ = [=](const std::size_t n, const T* const r, T* const x) {
            auto rowptr = crs->row_ptr->data();
            auto colidx = crs->col_idx->data();
            auto values = crs->values->data();
            assert(n == crs->rows());

            for (ptrdiff_t i = 0; i < n; i++) {
                const int extent = rowptr[i + 1] - rowptr[i];
                const T* row = &values[rowptr[i]];
                const idx_t* cols = &colidx[rowptr[i]];

                T acc = r[i];
                for (int k = 0; k < extent; k++) {
                    const idx_t j = cols[k];
                    const T aij = row[k];

                    acc -= aij * x[j];
                }

                x[i] += acc / d[i];
            }
        };

        gs->default_init();
        return gs;
    }

    // template <typename T>
    // std::shared_ptr<Smoother<T>> h_gauss_seidel(const ptrdiff_t n,
    //                                             const count_t* const rowptr,
    //                                             const idx_t* const colidx,
    //                                             const T* const values,
    //                                             const T* const d) {
    //     auto gs = std::make_shared<Smoother<T>>();

    //     gs->smooth_ = [=](const std::size_t n, const T* const r, T* const x) {
    //         for (ptrdiff_t i = 0; i < n; i++) {
    //             const int extent = rowptr[i + 1] - rowptr[i];
    //             const T* row = &values[rowptr[i]];
    //             const idx_t* cols = &colidx[rowptr[i]];

    //             T acc = r[i];
    //             for (int k = 0; k < extent; k++) {
    //                 const idx_t j = cols[k];
    //                 const T aij = row[k];

    //                 acc -= aij * x[j];
    //             }

    //             x[i] += acc / d[i];
    //         }
    //     };

    //     gs->set_n_dofs(n);
    //     gs->default_init();
    //     return gs;
    // }
}  // namespace sfem

#endif  // SFEM_GUASS_SEIDEL_HPP
