#ifndef __QUADRATURES_RULE_H__
#define __QUADRATURES_RULE_H__

// TRI3 6th order quadrature rule
#define TRI3_NQP 12
static real_t tri3_qw[TRI3_NQP] = {0.050844906370206816920936809106869,
                                   0.050844906370206816920936809106869,
                                   0.050844906370206816920936809106869,
                                   0.11678627572637936602528961138558,
                                   0.11678627572637936602528961138558,
                                   0.11678627572637936602528961138558,
                                   0.082851075618373575193553456420442,
                                   0.082851075618373575193553456420442,
                                   0.082851075618373575193553456420442,
                                   0.082851075618373575193553456420442,
                                   0.082851075618373575193553456420442,
                                   0.082851075618373575193553456420442};

static real_t tri3_qx[TRI3_NQP] = {0.063089014491502228340331602870819,
                                   0.063089014491502228340331602870819,
                                   0.87382197101699554331933679425836,
                                   0.24928674517091042129163855310702,
                                   0.24928674517091042129163855310702,
                                   0.50142650965817915741672289378596,
                                   0.053145049844816947353249671631398,
                                   0.053145049844816947353249671631398,
                                   0.31035245103378440541660773395655,
                                   0.31035245103378440541660773395655,
                                   0.63650249912139864723014259441205,
                                   0.63650249912139864723014259441205};

static real_t tri3_qy[TRI3_NQP] = {0.063089014491502228340331602870819,
                                   0.87382197101699554331933679425836,
                                   0.063089014491502228340331602870819,
                                   0.24928674517091042129163855310702,
                                   0.50142650965817915741672289378596,
                                   0.24928674517091042129163855310702,
                                   0.31035245103378440541660773395655,
                                   0.63650249912139864723014259441205,
                                   0.053145049844816947353249671631398,
                                   0.63650249912139864723014259441205,
                                   0.053145049844816947353249671631398,
                                   0.31035245103378440541660773395655};

#define EDGE2_NQP 12
static real_t edge2_qx[EDGE2_NQP] = {
    0.009219682876640489244124410106451250612735748291015625000000,
    0.047941371814762490100036984586040489375591278076171875000000,
    0.115048662902847487199409215463674627244472503662109375000000,
    0.206341022856691480580337838546256534755229949951171875000000,
    0.316084250500909991199449677878874354064464569091796875000000,
    0.437383295744265487847712847724324092268943786621093750000000,
    0.562616704255734512152287152275675907731056213378906250000000,
    0.683915749499089953289399090863298624753952026367187500000000,
    0.793658977143308463908510930195916444063186645507812500000000,
    0.884951337097152457289439553278498351573944091796875000000000,
    0.952058628185237454388811784156132489442825317382812500000000,
    0.990780317123359566267026821151375770568847656250000000000000};

static real_t edge2_qw[EDGE2_NQP] = {
    0.023587668193255501014604647025407757610082626342773437500000,
    0.053469662997658998215833037193078780546784400939941406250000,
    0.080039164271672999517726054818922420963644981384277343750000,
    0.101583713361533004015946346498822094872593879699707031250000,
    0.116746268269177499998789926394238136708736419677734375000000,
    0.124573522906701497636738906749087618663907051086425781250000,
    0.124573522906701497636738906749087618663907051086425781250000,
    0.116746268269177499998789926394238136708736419677734375000000,
    0.101583713361533004015946346498822094872593879699707031250000,
    0.080039164271672999517726054818922420963644981384277343750000,
    0.053469662997658998215833037193078780546784400939941406250000,
    0.023587668193255501014604647025407757610082626342773437500000};

#define SFEM_USE_RULE56
// #define SFEM_USE_RULE35
// #define SFEM_USE_RULE_STD45 

#ifdef SFEM_USE_RULE_STD45
#define TET4_NQP 45
static real_t tet4_qx[TET4_NQP] = {
    0.2500000000000000, 0.6175871903000830, 0.1274709365666390, 0.1274709365666390,
    0.1274709365666390, 0.9037635088221031, 0.0320788303926323, 0.0320788303926323,
    0.0320788303926323, 0.4502229043567190, 0.0497770956432810, 0.0497770956432810,
    0.0497770956432810, 0.4502229043567190, 0.4502229043567190, 0.3162695526014501,
    0.1837304473985499, 0.1837304473985499, 0.1837304473985499, 0.3162695526014501,
    0.3162695526014501, 0.0229177878448171, 0.2319010893971509, 0.2319010893971509,
    0.5132800333608811, 0.2319010893971509, 0.2319010893971509, 0.2319010893971509,
    0.0229177878448171, 0.5132800333608811, 0.2319010893971509, 0.0229177878448171,
    0.5132800333608811, 0.7303134278075384, 0.0379700484718286, 0.0379700484718286,
    0.1937464752488044, 0.0379700484718286, 0.0379700484718286, 0.0379700484718286,
    0.7303134278075384, 0.1937464752488044, 0.0379700484718286, 0.7303134278075384,
    0.1937464752488044};

static real_t tet4_qy[TET4_NQP] = {
    0.2500000000000000, 0.1274709365666390, 0.1274709365666390, 0.1274709365666390,
    0.6175871903000830, 0.0320788303926323, 0.0320788303926323, 0.0320788303926323,
    0.9037635088221031, 0.0497770956432810, 0.4502229043567190, 0.0497770956432810,
    0.4502229043567190, 0.0497770956432810, 0.4502229043567190, 0.1837304473985499,
    0.3162695526014501, 0.1837304473985499, 0.3162695526014501, 0.1837304473985499,
    0.3162695526014501, 0.2319010893971509, 0.0229177878448171, 0.2319010893971509,
    0.2319010893971509, 0.5132800333608811, 0.2319010893971509, 0.0229177878448171,
    0.5132800333608811, 0.2319010893971509, 0.5132800333608811, 0.2319010893971509,
    0.0229177878448171, 0.0379700484718286, 0.7303134278075384, 0.0379700484718286,
    0.0379700484718286, 0.1937464752488044, 0.0379700484718286, 0.7303134278075384,
    0.1937464752488044, 0.0379700484718286, 0.1937464752488044, 0.0379700484718286,
    0.7303134278075384};

static real_t tet4_qz[TET4_NQP] = {
    0.2500000000000000, 0.1274709365666390, 0.1274709365666390, 0.6175871903000830,
    0.1274709365666390, 0.0320788303926323, 0.0320788303926323, 0.9037635088221031,
    0.0320788303926323, 0.0497770956432810, 0.0497770956432810, 0.4502229043567190,
    0.4502229043567190, 0.4502229043567190, 0.0497770956432810, 0.1837304473985499,
    0.1837304473985499, 0.3162695526014501, 0.3162695526014501, 0.3162695526014501,
    0.1837304473985499, 0.2319010893971509, 0.2319010893971509, 0.0229177878448171,
    0.2319010893971509, 0.2319010893971509, 0.5132800333608811, 0.5132800333608811,
    0.2319010893971509, 0.0229177878448171, 0.0229177878448171, 0.5132800333608811,
    0.2319010893971509, 0.0379700484718286, 0.0379700484718286, 0.7303134278075384,
    0.0379700484718286, 0.0379700484718286, 0.1937464752488044, 0.1937464752488044,
    0.0379700484718286, 0.7303134278075384, 0.7303134278075384, 0.1937464752488044,
    0.0379700484718286};

static real_t tet4_qw[TET4_NQP] = {
    -0.2359620398477559, 0.0244878963560563, 0.0244878963560563, 0.0244878963560563,
    0.0244878963560563,  0.0039485206398261, 0.0039485206398261, 0.0039485206398261,
    0.0039485206398261,  0.0263055529507371, 0.0263055529507371, 0.0263055529507371,
    0.0263055529507371,  0.0263055529507371, 0.0263055529507371, 0.0829803830550590,
    0.0829803830550590,  0.0829803830550590, 0.0829803830550590, 0.0829803830550590,
    0.0829803830550590,  0.0254426245481024, 0.0254426245481024, 0.0254426245481024,
    0.0254426245481024,  0.0254426245481024, 0.0254426245481024, 0.0254426245481024,
    0.0254426245481024,  0.0254426245481024, 0.0254426245481024, 0.0254426245481024,
    0.0254426245481024,  0.0134324384376852, 0.0134324384376852, 0.0134324384376852,
    0.0134324384376852,  0.0134324384376852, 0.0134324384376852, 0.0134324384376852,
    0.0134324384376852,  0.0134324384376852, 0.0134324384376852, 0.0134324384376852,
    0.0134324384376852};

#endif  // SFEM_USE_RULE_STD45

#ifdef SFEM_USE_RULE35

#define TET4_NQP 35
static real_t tet4_qw[TET4_NQP] = {
    0.0021900463965388, 0.0021900463965388, 0.0021900463965388, 0.0021900463965388,
    0.0143395670177665, 0.0143395670177665, 0.0143395670177665, 0.0143395670177665,
    0.0143395670177665, 0.0143395670177665, 0.0143395670177665, 0.0143395670177665,
    0.0143395670177665, 0.0143395670177665, 0.0143395670177665, 0.0143395670177665,
    0.0250305395686746, 0.0250305395686746, 0.0250305395686746, 0.0250305395686746,
    0.0250305395686746, 0.0250305395686746, 0.0479839333057554, 0.0479839333057554,
    0.0479839333057554, 0.0479839333057554, 0.0479839333057554, 0.0479839333057554,
    0.0479839333057554, 0.0479839333057554, 0.0479839333057554, 0.0479839333057554,
    0.0479839333057554, 0.0479839333057554, 0.0931745731195340};

static real_t tet4_qx[TET4_NQP] = {
    0.0267367755543735, 0.9197896733368801, 0.0267367755543735, 0.0267367755543735,
    0.7477598884818091, 0.1740356302468940, 0.0391022406356488, 0.0391022406356488,
    0.0391022406356488, 0.0391022406356488, 0.1740356302468940, 0.7477598884818091,
    0.1740356302468940, 0.7477598884818091, 0.0391022406356488, 0.0391022406356488,
    0.4547545999844830, 0.0452454000155172, 0.0452454000155172, 0.4547545999844830,
    0.4547545999844830, 0.0452454000155172, 0.2232010379623150, 0.5031186450145980,
    0.2232010379623150, 0.2232010379623150, 0.5031186450145980, 0.2232010379623150,
    0.0504792790607720, 0.0504792790607720, 0.0504792790607720, 0.5031186450145980,
    0.2232010379623150, 0.2232010379623150, 0.2500000000000000};

static real_t tet4_qy[TET4_NQP] = {
    0.0267367755543735, 0.0267367755543735, 0.9197896733368801, 0.0267367755543735,
    0.0391022406356488, 0.0391022406356488, 0.7477598884818091, 0.1740356302468940,
    0.0391022406356488, 0.0391022406356488, 0.7477598884818091, 0.1740356302468940,
    0.0391022406356488, 0.0391022406356488, 0.1740356302468940, 0.7477598884818091,
    0.0452454000155172, 0.4547545999844830, 0.0452454000155172, 0.4547545999844830,
    0.0452454000155172, 0.4547545999844830, 0.2232010379623150, 0.2232010379623150,
    0.5031186450145980, 0.0504792790607720, 0.0504792790607720, 0.0504792790607720,
    0.2232010379623150, 0.5031186450145980, 0.2232010379623150, 0.2232010379623150,
    0.5031186450145980, 0.2232010379623150, 0.2500000000000000};

static real_t tet4_qz[TET4_NQP] = {
    0.0267367755543735, 0.0267367755543735, 0.0267367755543735, 0.9197896733368801,
    0.0391022406356488, 0.0391022406356488, 0.0391022406356488, 0.0391022406356488,
    0.7477598884818091, 0.1740356302468940, 0.0391022406356488, 0.0391022406356488,
    0.7477598884818091, 0.1740356302468940, 0.7477598884818091, 0.1740356302468940,
    0.0452454000155172, 0.0452454000155172, 0.4547545999844830, 0.0452454000155172,
    0.4547545999844830, 0.4547545999844830, 0.0504792790607720, 0.0504792790607720,
    0.0504792790607720, 0.2232010379623150, 0.2232010379623150, 0.5031186450145980,
    0.2232010379623150, 0.2232010379623150, 0.5031186450145980, 0.2232010379623150,
    0.2232010379623150, 0.5031186450145980, 0.2500000000000000};

#endif  // SFEM_USE_RULE35

#ifdef SFEM_USE_RULE56
#define TET4_NQP 56
static real_t tet4_qw[TET4_NQP] = {
    0.0010373112336140, 0.0010373112336140, 0.0010373112336140, 0.0010373112336140,
    0.0096016645399480, 0.0096016645399480, 0.0096016645399480, 0.0096016645399480,
    0.0096016645399480, 0.0096016645399480, 0.0096016645399480, 0.0096016645399480,
    0.0096016645399480, 0.0096016645399480, 0.0096016645399480, 0.0096016645399480,
    0.0164493976798232, 0.0164493976798232, 0.0164493976798232, 0.0164493976798232,
    0.0164493976798232, 0.0164493976798232, 0.0164493976798232, 0.0164493976798232,
    0.0164493976798232, 0.0164493976798232, 0.0164493976798232, 0.0164493976798232,
    0.0153747766513310, 0.0153747766513310, 0.0153747766513310, 0.0153747766513310,
    0.0153747766513310, 0.0153747766513310, 0.0153747766513310, 0.0153747766513310,
    0.0153747766513310, 0.0153747766513310, 0.0153747766513310, 0.0153747766513310,
    0.0293520118375230, 0.0293520118375230, 0.0293520118375230, 0.0293520118375230,
    0.0293520118375230, 0.0293520118375230, 0.0293520118375230, 0.0293520118375230,
    0.0293520118375230, 0.0293520118375230, 0.0293520118375230, 0.0293520118375230,
    0.0366291366405108, 0.0366291366405108, 0.0366291366405108, 0.0366291366405108};

static real_t tet4_qx[TET4_NQP] = {
    0.0149520651530592, 0.9551438045408220, 0.0149520651530592, 0.0149520651530592,
    0.1518319491659370, 0.7799760084415400, 0.0340960211962615, 0.0340960211962615,
    0.0340960211962615, 0.0340960211962615, 0.7799760084415400, 0.1518319491659370,
    0.7799760084415400, 0.1518319491659370, 0.0340960211962615, 0.0340960211962615,
    0.5526556431060171, 0.3549340560639790, 0.0462051504150017, 0.0462051504150017,
    0.0462051504150017, 0.0462051504150017, 0.3549340560639790, 0.5526556431060171,
    0.3549340560639790, 0.5526556431060171, 0.0462051504150017, 0.0462051504150017,
    0.2281904610687610, 0.5381043228880020, 0.2281904610687610, 0.2281904610687610,
    0.5381043228880020, 0.2281904610687610, 0.0055147549744775, 0.0055147549744775,
    0.0055147549744775, 0.5381043228880020, 0.2281904610687610, 0.2281904610687610,
    0.3523052600879940, 0.1961837595745600, 0.3523052600879940, 0.3523052600879940,
    0.1961837595745600, 0.3523052600879940, 0.0992057202494530, 0.0992057202494530,
    0.0992057202494530, 0.1961837595745600, 0.3523052600879940, 0.3523052600879940,
    0.1344783347929940, 0.5965649956210171, 0.1344783347929940, 0.1344783347929940};

static real_t tet4_qy[TET4_NQP] = {
    0.0149520651530592, 0.0149520651530592, 0.9551438045408220, 0.0149520651530592,
    0.0340960211962615, 0.0340960211962615, 0.1518319491659370, 0.7799760084415400,
    0.0340960211962615, 0.0340960211962615, 0.1518319491659370, 0.7799760084415400,
    0.0340960211962615, 0.0340960211962615, 0.7799760084415400, 0.1518319491659370,
    0.0462051504150017, 0.0462051504150017, 0.5526556431060171, 0.3549340560639790,
    0.0462051504150017, 0.0462051504150017, 0.5526556431060171, 0.3549340560639790,
    0.0462051504150017, 0.0462051504150017, 0.3549340560639790, 0.5526556431060171,
    0.2281904610687610, 0.2281904610687610, 0.5381043228880020, 0.0055147549744775,
    0.0055147549744775, 0.0055147549744775, 0.2281904610687610, 0.5381043228880020,
    0.2281904610687610, 0.2281904610687610, 0.5381043228880020, 0.2281904610687610,
    0.3523052600879940, 0.3523052600879940, 0.1961837595745600, 0.0992057202494530,
    0.0992057202494530, 0.0992057202494530, 0.3523052600879940, 0.1961837595745600,
    0.3523052600879940, 0.3523052600879940, 0.1961837595745600, 0.3523052600879940,
    0.1344783347929940, 0.1344783347929940, 0.5965649956210171, 0.1344783347929940};

static real_t tet4_qz[TET4_NQP] = {
    0.0149520651530592, 0.0149520651530592, 0.0149520651530592, 0.9551438045408220,
    0.0340960211962615, 0.0340960211962615, 0.0340960211962615, 0.0340960211962615,
    0.1518319491659370, 0.7799760084415400, 0.0340960211962615, 0.0340960211962615,
    0.1518319491659370, 0.7799760084415400, 0.1518319491659370, 0.7799760084415400,
    0.0462051504150017, 0.0462051504150017, 0.0462051504150017, 0.0462051504150017,
    0.5526556431060171, 0.3549340560639790, 0.0462051504150017, 0.0462051504150017,
    0.5526556431060171, 0.3549340560639790, 0.5526556431060171, 0.3549340560639790,
    0.0055147549744775, 0.0055147549744775, 0.0055147549744775, 0.2281904610687610,
    0.2281904610687610, 0.5381043228880020, 0.2281904610687610, 0.2281904610687610,
    0.5381043228880020, 0.2281904610687610, 0.2281904610687610, 0.5381043228880020,
    0.0992057202494530, 0.0992057202494530, 0.0992057202494530, 0.3523052600879940,
    0.3523052600879940, 0.1961837595745600, 0.3523052600879940, 0.3523052600879940,
    0.1961837595745600, 0.3523052600879940, 0.3523052600879940, 0.1961837595745600,
    0.1344783347929940, 0.1344783347929940, 0.1344783347929940, 0.5965649956210171};

#endif  // SFEM_USE_RULE56

#endif  // __QUADRATURES_RULE_H__