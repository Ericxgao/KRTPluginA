#include "plugin.hpp"


struct F : Module {
	enum ParamIds {
		SPD,
		SKW,
		FRQ,
		LAH,
		DRV,
		INV,
		NUM_PARAMS
	};
	enum InputIds {
		ISPD,
		ISKW,
		IFRQ,
		ILAH,
		IDRV,
		IINV,
		IN,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	int maxPoly() {
		int poly = 1;
		for(int i = 0; i < NUM_INPUTS; i++) {
			int chan = inputs[i].getChannels();
			if(chan > poly) poly = chan;
		}
		for(int o = 0; o < NUM_OUTPUTS; o++) {
			outputs[o].setChannels(poly);
		}
		return poly;
	}

	//KK constants
	//s^2+k1*s+k2
	float kk[8][2] = {
		{ sqrtf(2.f), 1.f },//BW2
		{ sqrtf(2.f), 1.f },//BW2
		{ 0.765367f, 1.f },//BW4/2
		{ 1.847759f, 1.f },//BW4/2
		{ 5.79242f, 9.140131f },//BE4/2
		{ 4.20758f, 6.45943f },//BE4/2
		{ 0.46338f, 0.947669f },//L4/2
		{ 1.09948f, 0.430787f },//L4/2
	};

	//LPF standard transforms
	float freqMul(int filt) {
		//s^2
		float s2 = 1.f / kk[filt][1];//divide through
		return sqrtf(s2);//root for s multiplier
	}

	float findK(int filt) {
		//s
		float s = kk[filt][0] / freqMul(filt);
		return s;//damping effective given frequecy shift
	}

	float freqMul(float spd, float skw, int add) {//add == 0 or 1
		float x = freqMul(add) * ((spd - 1.f) * -0.5f) + freqMul(add + 2) * ((spd + 1.f) * 0.5f);
		float y = freqMul(add + 4) * ((spd - 1.f) * -0.5f) + freqMul(add + 6) * ((spd + 1.f) * 0.5f);
		float z = x * ((skw - 1.f) * -0.5f) + y * ((skw + 1.f) * 0.5f);
		return z;
	}

	float findK(float spd, float skw, int add) {//add == 0 or 1
		float x = findK(add) * ((spd - 1.f) * -0.5f) + findK(add + 2) * ((spd + 1.f) * 0.5f);
		float y = findK(add + 4) * ((spd - 1.f) * -0.5f) + findK(add + 6) * ((spd + 1.f) * 0.5f);
		float z = x * ((skw - 1.f) * -0.5f) + y * ((skw + 1.f) * 0.5f);
		return z;
	}

	float f, t, u, k, tf, bl[PORT_MAX_CHANNELS][2], bb[PORT_MAX_CHANNELS][2];

    /* 2P
           Ghigh * s^2 + Gband * s + Glow
    H(s) = ------------------------------
                 s^2 + k * s + 1
     */
    //TWO POLE FILTER
	void setFK2(float fc, float kd, float fs) {
		f   = tanf(M_PI * fc / fs);
		k   = kd;
		t   = 1 / (1 + k * f);
		u   = 1 / (1 + t * f * f);
		tf  = t * f;
	}

	float process2(float in, int p, float lah, float inv, int add, float drv) {
		float low = (bl[p][add] + tf * (bb[p][add] + f * in)) * u;
		float band = (bb[p][add] + f * (in - low)) * t;
		float high = in - low - k * band;
		//TODO still needs energy clamp!!
		bb[p][add] = clomp(band + f * high, drv);
		bl[p][add] = clomp(low  + f * band, drv);
		lah = low * ((lah - 1.f) * -0.5f) + high * ((lah + 1.f) * 0.5f);
		return lah * ((inv - 1.f) * -0.5f) + (in - lah) * ((inv + 1.f) * 0.5f);
	}

	float clomp(float in, float drv) {
		return in;
	}

	F() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SPD, -1.f, 1.f, 1.f, "Spread");
		configParam(SKW, -1.f, 1.f, -1.f, "Skew");
		configParam(FRQ, -4.f, 4.f, 0.f, "Frequency", " Oct");
		configParam(LAH, -1.f, 1.f, -1.f, "Low High");
		configParam(DRV, -1.f, 1.f, 0.f, "Drive");
		configParam(INV, -1.f, 1.f, -1.f, "Invert");
	}

	//obtain mapped control value
    float log(float val, float centre) {
        return powf(2.f, val) * centre;
    }

	void process(const ProcessArgs& args) override {
		float fs = args.sampleRate;
		int maxPort = maxPoly();

		float spd = params[SPD].getValue();
		float skw = params[SKW].getValue();
		float frq = params[FRQ].getValue();
		float lah = params[LAH].getValue();
		float drv = params[DRV].getValue();
		float inv = params[INV].getValue();

		// PARAMETERS (AND IMPLICIT INS)
#pragma GCC ivdep
		for(int p = 0; p < maxPort; p++) {
			float ispd = inputs[ISPD].getPolyVoltage(p) + spd;
			float iskw = inputs[ISKW].getPolyVoltage(p) + skw;
			float ifrq = log(inputs[IFRQ].getPolyVoltage(p) + frq, dsp::FREQ_C4);
			float flo0 = freqMul(ispd, iskw, 0);//first
			float flo1 = freqMul(ispd, iskw, 1);//second
			float damp0 = findK(ispd, iskw, 0);//first
			float damp1 = findK(ispd, iskw, 1);//second

			flo0 = clamp(ifrq * flo0, 0.f, fs * 0.5f);
			flo1 = clamp(ifrq * flo1, 0.f, fs * 0.5f);
			//calm max change
			float ilah = inputs[ILAH].getPolyVoltage(p) * 0.1f + lah;
			float idrv = inputs[IDRV].getPolyVoltage(p) * 0.1f + drv;
			float iinv = inputs[IINV].getPolyVoltage(p) * 0.1f + inv;

			// IN
			float in = inputs[IN].getPolyVoltage(p);
			setFK2(flo0, damp0, fs);
			in = process2(in, p, ilah, iinv, 0, idrv);
			setFK2(flo1, damp1, fs);
			in = process2(in, p, ilah, iinv, 1, idrv);

			// OUT
			outputs[OUT].setVoltage(in, p);
		}
	}
};

//geometry edit
#define HP 5
#define LANES 2
#define RUNGS 7

//ok
#define HP_UNIT 5.08
#define WIDTH (HP*HP_UNIT)
#define X_SPLIT (WIDTH / 2.f / LANES)

#define HEIGHT 128.5
#define Y_MARGIN 0.05f
#define R_HEIGHT (HEIGHT*(1-2*Y_MARGIN))
#define Y_SPLIT (R_HEIGHT / 2.f / RUNGS)

//placement macro
#define loc(x,y) mm2px(Vec(X_SPLIT*(1+2*(x-1)), (HEIGHT*Y_MARGIN)+Y_SPLIT*(1+2*(y-1))))

struct FWidget : ModuleWidget {
	FWidget(F* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/F.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(loc(1, 1), module, F::SPD));
		addParam(createParamCentered<RoundBlackKnob>(loc(2, 1), module, F::SKW));
		addParam(createParamCentered<RoundBlackKnob>(loc(1, 2), module, F::FRQ));
		addParam(createParamCentered<RoundBlackKnob>(loc(2, 2), module, F::LAH));
		addParam(createParamCentered<RoundBlackKnob>(loc(1, 3), module, F::INV));
		addParam(createParamCentered<RoundBlackKnob>(loc(2, 3), module, F::DRV));

		addInput(createInputCentered<PJ301MPort>(loc(1, 4), module, F::ISPD));
		addInput(createInputCentered<PJ301MPort>(loc(2, 4), module, F::ISKW));
		addInput(createInputCentered<PJ301MPort>(loc(1, 5), module, F::IFRQ));
		addInput(createInputCentered<PJ301MPort>(loc(2, 5), module, F::ILAH));
		addInput(createInputCentered<PJ301MPort>(loc(1, 6), module, F::IINV));
		addInput(createInputCentered<PJ301MPort>(loc(2, 6), module, F::IDRV));

		addInput(createInputCentered<PJ301MPort>(loc(1, 7), module, F::IN));
		addOutput(createOutputCentered<PJ301MPort>(loc(2, 7), module, F::OUT));
	}
};


Model* modelF = createModel<F, FWidget>("F");