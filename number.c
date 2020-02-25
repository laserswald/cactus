

struct cact_rat {
    z_t num;
    z_t den;
};

struct cact_cplx {
	struct cact_rat *real;
	struct cact_rat *imag;
}

struct cact_num {

};

struct cact_rat cact_num_div()
