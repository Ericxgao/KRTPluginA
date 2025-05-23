#include "plugin.hpp"

//Z = C4 note ...

struct Y : Module {
	enum ParamIds {
		ENUMS(QUADS, 16),
		ENUMS(TRIPS, 12),
		RUN,
		RST,
		TEMPO,
		ENUMS(MODES, 4),
		LEN,
		IS_RUN,
		MODE,
		ENUMS(PAT, 16),
		CPY,
		PST,
		CHAN,
		CPAT,
		CCHN,
		ENUMS(MUTES, 16),
		JAZZ3,
		JAZZ4,
		NUM_PARAMS
	};
	enum InputIds {
		ICV_BUT,
		IGATE_BUT,
		IPOS,
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUTS, 16),
		ORUN,
		ORST,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(LQUADS, 16 * 2),
		ENUMS(LTRIPS, 12 * 2),
		LRUN,
		LRST,
		ENUMS(LMODE, 4),
		LCPY,
		LPST,
		NUM_LIGHTS
	};

	const char *instring[NUM_INPUTS] = {
		"Control command select",
		"Control execute",
		"Position scroll",
	};

	const char *outstring[NUM_OUTPUTS] = {
		"Trigger 1",
		"Trigger 2",
		"Trigger 3",
		"Trigger 4",
		"Trigger 5",
		"Trigger 6",
		"Trigger 7",
		"Trigger 8",
		"Trigger 9",
		"Trigger 10",
		"Trigger 11",
		"Trigger 12",
		"Trigger 13",
		"Trigger 14",
		"Trigger 15",
		"Trigger 16",
		"Run clock",
		"Reset timing",
	};

	const char *lightstring[NUM_LIGHTS] = {
		//no use ...
		//done by buttons, but ... RGB tripple tool tip faux pas.
	};

	void iol(bool lights) {
		for(int i = 0; i < NUM_INPUTS; i++) configInput(i, instring[i]);
		for(int i = 0; i < NUM_OUTPUTS; i++) configOutput(i, outstring[i]);
		if(!lights) return;
		for(int i = 0; i < NUM_LIGHTS; i++) configLight(i, lightstring[i]);
	}

#define T 16
#define Q 32
#define CPYA 64
#define PSTA 128
#define RUNA 256
#define RSTA 512
#define PATA 1024
#define SEQA 2048
#define MUTA 4096
#define NOWA 8192
#define TR(x) T + x
#define QU(x) Q + x

	const int noteProcess[128] = {
	//	C				D				E		F				G				A				B
		0,		0, 		0, 		0,		0,		0,		0,		0,		0,		0,		0,		0,
		0,		0, 		0, 		0,		0,		0,		0,		0,		0,		0,		0,		0,
		0,		0, 		0, 		0,		0,		0,		0,		0,		0,		0,		0,		0,
	TR(0),	 CPYA, 	TR(1),	 PSTA,	TR(2),	QU(0),	TR(0),	QU(1),	TR(1),	QU(2),	TR(2),	QU(3),
	TR(3),	 PATA, 	TR(4), 	 SEQA,	TR(5),	QU(4),	TR(3),	QU(5),	TR(4),	QU(6),	TR(5),	QU(7),
		//C4 starts next row
	TR(6),	 MUTA, 	TR(7), 	 NOWA,	TR(8),	QU(8),	TR(6),	QU(9),	TR(7), QU(10),	TR(8), QU(11),
	TR(9),	 RSTA, TR(10), 	 RUNA,	TR(11), QU(12),	TR(9), QU(13), TR(10), QU(14), TR(11), QU(15),
		0,		0, 		0, 		0,		0,		0,		0,		0,		0,		0,		0,		0,
		0,		0, 		0, 		0,		0,		0,		0,		0,		0,		0,		0,		0,
		0,		0, 		0, 		0,		0,		0,		0,		0,		0,		0,		0,		0,
		0,		0, 		0, 		0,		0,		0,		0,		0
	};

	const static int patNum = 16;
	const static int stepsNum = 16 + 12;
	const static int chanNum = 16;

	bool patterns[patNum][stepsNum][chanNum];

	const static int sized = chanNum * stepsNum * patNum;
	char saves[sized];//space

	json_t* dataToJson() override {
		json_t *rootJ = json_object();
		for(int p = 0; p < patNum; p++) {
			for(int s = 0; s < stepsNum; s++) {
				for(int c = 0; c < chanNum; c++) {
					int i = c + chanNum * (s + stepsNum * p);
					saves[i] = patterns[p][s][c] ? 'T' : 'F';
				}
			}
		}
		json_object_set_new(rootJ, "save", json_stringn(saves, sized));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* textJ = json_object_get(rootJ, "save");
  		if (textJ) {
			const char *str = json_string_value(textJ);
			if(str) {
				for(int p = 0; p < patNum; p++) {
					for(int s = 0; s < stepsNum; s++) {
						for(int c = 0; c < chanNum; c++) {
							int i = c + chanNum * (s + stepsNum * p);
							patterns[p][s][c] = (str[i] == 'T') ? true : false;
						}
					}
				}
			}
		}
	}

	void maxPolySpecial() {
#pragma GCC ivdep
		for(int o = 0; o < NUM_OUTPUTS; o++) {
			outputs[o].setChannels(1);
		}
	}

	const char qNames[16][4] = {
		"1/1", "2/1", "3/1", "4/1",
		"1/2", "2/2", "3/2", "4/2",
		"1/3", "2/3", "3/3", "4/3",
		"1/4", "2/4", "3/4", "4/4"
	};

	const char tNames[12][5] = {
		"1T/1", "2T/1", "3T/1",
		"1T/2", "2T/2", "3T/2",
		"1T/3", "2T/3", "3T/3",
		"1T/4", "2T/4", "3T/4"
	};

	const char *mNames[4] = {
		"Pattern", "Sequence", "Mute", "Now"
	};

	Y() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TEMPO, 0.f, 240.f, 120.f, "Tempo", " bpm");
		configParam(LEN, 0.f, 100.f, 50.f, "Gate length", " %");
		for(int i = 0; i < 16; i++) {
			configButton(QUADS + i, qNames[i]);
		}
		for(int i = 0; i < 12; i++) {
			configButton(TRIPS + i, tNames[i]);
		}
		configButton(RUN, "Run and stop");
		configButton(RST, "Reset");
		for(int i = 0; i < 4; i++) {
			configButton(MODES + i, mNames[i]);
		}
		configParam(IS_RUN, 0.f, 1.f, 0.f);
		configSwitch(MODE, 0.f, 3.f, 0.f);
		for(int i = 0; i < 16; i++) {
			configSwitch(PAT + i, 0.f, patNum - 1.f, 0.f);//default pattern
			configSwitch(MUTES + i, 0.f, 1.f, 1.f);
		}
		configButton(CPY, "Copy");
		configButton(PST, "Paste");
		configSwitch(CHAN, 0.f, chanNum - 1.f, 0.f);
		for(int p = 0; p < patNum; p++) {
			for(int s = 0; s < stepsNum; s++) {
				for(int c = 0; c < chanNum; c++) {
					patterns[p][s][c] = false;//blank
				}
			}
		}
		configSwitch(CPAT, 0.f, 15.f, 0.f);
		configSwitch(CCHN, 0.f, 15.f, 0.f);
		configParam(JAZZ4, -1.f, 1.f, 0.f, "Quad jazz");
		configParam(JAZZ3, -1.f, 1.f, 0.f, "Triple jazz");
		iol(false);//lights thru buttons as RGB *3 overlay happens????
	}

	double beatCounter = 0;
	dsp::SchmittTrigger sRun;
	dsp::SchmittTrigger sRst;
	dsp::SchmittTrigger sCpy;
	dsp::SchmittTrigger sPst;
	dsp::SchmittTrigger mode[4];
	dsp::SchmittTrigger quads[16];
	dsp::SchmittTrigger trips[12];
	dsp::SchmittTrigger sGate[PORT_MAX_CHANNELS];

#define MODE_PAT 0
#define MODE_SEQ 1
#define MODE_MUT 2
#define MODE_NOW 3

	int mux = 0;

	void light4(float beat, int light, int mode) {
		if(mode == MODE_PAT) {
			int p = getPat(beat);
			int chan = params[CHAN].getValue();
			bool on = (patterns[p][light][chan]) == true;
			bool in = ((int)beat & 15) == light;
			GRLed(LQUADS, light, in, on);//red set, green tick
		} else {//rest
			int p = getPat(beat);
			bool in = (patterns[p][(int)beat & 15][light]) == true;
			bool on = (p == light) && (mode == MODE_SEQ);
			on |= (params[MUTES + light].getValue() < 0.5f) && (mode == MODE_MUT);
			on |= ((int)params[CHAN].getValue() == light) && (mode == MODE_NOW);
			GRLed(LQUADS, light, in, on);//red set, green tick
		}
	}

	void button4(float beat, int button, int mode) {
		if(mode > 1) {
			if(mode > 2) {//MODE_NOW
				//directly done
				params[CHAN].setValue(button);//set channel
			} else {//MODE_MUT
				float x = params[MUTES + button].getValue();
				params[MUTES + button].setValue(1.f - x);//set mute
			}
		} else {
			if(mode < 1) {//MODE_PAT
				int p = getPat(beat);
				int chan = params[CHAN].getValue();
				(patterns[p][button][chan]) ^= true;//flip
			} else {//MODE_SEQ
				//12 pattern quad for 64 mux
				int pi = (int)beat >> 4;
				int p =	params[PAT + 12 + pi].getValue();//indirect on selected
				p = mod3[p];//for next choice
				params[PAT + p + 3 * pi].setValue(button);
			}
		}
	}

	char mod12[48] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
	};

	char mod3[16] = {
		0, 1, 2, 0, 1, 2, 0, 1, 2,
		0, 1, 2, 0, 1, 2, 0
	};

	char div3[12] = {
		0, 0, 0, 1, 1, 1, 2, 2, 2,
		3, 3, 3
	};

	int getPat(float beat) {
		//12 pattern quad for 64 mux
		int pi = (int)beat >> 4;
		int p =	params[PAT + 12 + pi].getValue();//indirect on selected
		p = mod3[p];//for next choice
		return params[PAT + p + 3 * pi].getValue();//indirect from choices
	}

	float out43(float beat, float tBeat, int out, int mode) {
		int p = getPat(beat);
		float l = params[LEN].getValue() * 0.01f;
		bool q = patterns[p][(int)beat & 15][out];
		q &= onLen(beat, l);
		bool t = patterns[p][mod12[(int)tBeat] + 16][out];
		t &= onLen(tBeat, l);
		t |= q;//is on?
		if(mode == MODE_NOW) {
			t |= params[QUADS + out].getValue() > 0.5f;//play now
		}
		return (t ? 10.f : 0.f) * params[MUTES + out].getValue();//gate
	}

	void light3(float beat, float tBeat, int light, int mode) {
		if(mode == MODE_PAT) {
			int p = getPat(beat);
			int chan = params[CHAN].getValue();
			bool on = (patterns[p][light + 16][chan]) == true;
			bool in = mod12[(int)tBeat] == light;
			GRLed(LTRIPS, light, in, on);//red set, green tick
		} else {//rest
			int r = mod3[light];//button in set
			int q = div3[light];//div 3
			bool on = ((int)params[PAT + 12 + q].getValue() == r);//0 - 2
			bool in = q == (int)beat >> 4;
			GRLed(LTRIPS, light, in, on);//red set, green tick
		}
	}

	void button3(float beat, int button, int mode) {
		if(mode == MODE_PAT) {
			int p = getPat(beat);
			int chan = params[CHAN].getValue();
			(patterns[p][button + 16][chan]) ^= true;//flip
		} else {//rest
			int r = mod3[button];//button in set
			int q = div3[button];//div 3
			params[PAT + 12 + q].setValue(r);//0 - 2
		}
	}

	bool onLen(float beats, float len) {
		return beats - (int)beats < len;
	}

	//double bit error ?? time??
	double modulo(double x, float m) {
		double div = x / m;
		long d = (long) div;
		double rem = x - d * m;
		return rem;
	}

	void GRLed(int base, int id, bool g, bool r) {
		lights[base + 2 * id].setBrightness(g ? 1.f : 0.f);
		lights[base + 2 * id + 1].setBrightness(r ? 1.f : 0.f);
	}

	int actionCodes[PORT_MAX_CHANNELS];
	int codeCount = 0;

	void clearActions() {
#pragma GCC ivdep
		for(int i = 0; i < PORT_MAX_CHANNELS; i++) {
			actionCodes[i] = 0;
		}
		codeCount = 0;
	}

	void setAction(int action) {
		if (codeCount < PORT_MAX_CHANNELS)
			actionCodes[codeCount++] = action;
	}

	bool getAction(int action) {
		bool out = false;
#pragma GCC ivdep
		for(int i = 0; i < PORT_MAX_CHANNELS; i++) {
			if(actionCodes[i] == action) out |= true;
		}
		return out;
	}

	void process(const ProcessArgs& args) override {
		mux++;
		float fs = args.sampleRate;
		maxPolySpecial();//1
		double bps = (double)params[TEMPO].getValue() / 15.f;//beat per bar
		double beatSamp = bps / fs;//beats per sample
		float pos = inputs[IPOS].getVoltage() / 10.f * 64.f;
		float beats = modulo(beatCounter + abs(pos + 64.f), 64);
		float tBeats = beats * 0.75f;//triples
		float rst = params[RST].getValue();
		bool trigRst = sRst.process(rst);
		float run = params[RUN].getValue();
		bool trigRun = sRun.process(run);
		float cpy = params[CPY].getValue();
		bool trigCpy = sCpy.process(cpy);
		float pst = params[PST].getValue();
		bool trigPst = sPst.process(pst);
#pragma GCC ivdep
		for(int i = 0; i < PORT_MAX_CHANNELS; i++) {
			float gate = inputs[IGATE_BUT].getPolyVoltage(i);
			float cv = inputs[ICV_BUT].getPolyVoltage(i);
			bool trigGate = sGate[i].process(rescale(gate, 0.1f, 2.f, 0.f, 1.f));
			if(trigGate) {//set action code
				float tcv = cv * 12.f;
				tcv += 0.5f + 60.f;
				int icv = (int)tcv;
				if(icv >= 0 && icv <= 127) {
					setAction(noteProcess[icv]);
				}
			}
		}
		//jazz (parabolic if free)
		float jazz3 = params[JAZZ3].getValue();
		float mBeats = modulo(tBeats, 3);
		tBeats += jazz3 * 0.32f * mBeats * (mBeats - 3.f);//parabolic retiming
		float jazz4 = params[JAZZ4].getValue();
		float nBeats = modulo(beats, 2);
		beats += jazz4 * 0.49f * nBeats * (nBeats - 2.f);//parabolic retiming
		int newMode = params[MODE].getValue();//old
#pragma GCC ivdep
		for(int i = 0; i < 4; i++) {
			float m = params[MODES + i].getValue();//buttons
			bool trigM = mode[i].process(m);
			if(trigM) {
				newMode = i;//set new
			}
			if(getAction(SEQA)) newMode = MODE_SEQ;
			if(getAction(PATA)) newMode = MODE_PAT;
			if(getAction(MUTA)) newMode = MODE_MUT;
			if(getAction(NOWA)) newMode = MODE_NOW;
			if((mux & 1023) == 0) lights[LMODE + i].setBrightness((newMode == i) ? 1.f : 0.f);//radios
		}
		params[MODE].setValue(newMode);//change
		if(trigRst || getAction(RSTA)) {//sanity range before use
			beatCounter = 0.f;//beats long
			beats = 0.f;//faster and sample accurate
			tBeats = 0.f;
		}
#pragma GCC ivdep
		for(int i = 0; i < 16; i++) {
			float but = params[QUADS + i].getValue();
			bool trig = quads[i].process(but);
			if(trig || getAction(QU(i))) {
				button4(beats, i, newMode);
			}
			if((mux & 1023) == 1) light4(beats, i, newMode);
			outputs[OUTS + i].setVoltage(out43(beats, tBeats, i, newMode));
		}
#pragma GCC ivdep
		for(int i = 0; i < 12; i++) {
			float but = params[TRIPS + i].getValue();
			bool trig = trips[i].process(but);
			if(trig || getAction(TR(i))) {
				button3(beats, i, newMode);
			}
			if((mux & 1023) == 2) light3(beats, tBeats, i, newMode);
		}
		if(trigRun || getAction(RUNA)) {
			params[IS_RUN].setValue(1.f - params[IS_RUN].getValue());//ok?
		}
		if(trigCpy || getAction(CPYA)) {
			params[CPAT].setValue(getPat(beats));
			params[CCHN].setValue(params[CHAN].getValue());
		}
		if((mux & 1023) == 8) lights[LCPY].setBrightness(getPat(beats) == (int)params[CPAT].getValue() ? 1.f : 0.f);
		if(trigPst || getAction(PSTA)) {
			int pat = params[CPAT].getValue();
			int chan = params[CCHN].getValue();
			int nPat = getPat(beats);
			int nChan = params[CHAN].getValue();
			if(newMode == MODE_PAT) {
				//single chan
#pragma GCC ivdep
				for(int i = 0; i < stepsNum; i++) {
					patterns[nPat][i][nChan] = patterns[pat][i][chan];
				}
			} else {
				//full pattern
#pragma GCC ivdep
				for(int j = 0; j < chanNum; j++) {
					for(int i = 0; i < stepsNum; i++) {
						patterns[nPat][i][j] = patterns[pat][i][j];
					}
				}
			}
		}
		if((mux & 1023) == 3) {
			bool ok = newMode != MODE_PAT;
			//light off if different channel in channel paste
			ok |= (int)params[CCHN].getValue() == (int)params[CHAN].getValue();
			lights[LPST].setBrightness(ok ? 1.f : 0.f);
		}
		float len = params[LEN].getValue() * 0.01f;
		if(onLen(beats, len)) {
			outputs[ORUN].setVoltage(10.f);
			if((mux & 1023) == 4) lights[LRUN].setBrightness(1.f);
		} else {
			outputs[ORUN].setVoltage(0.f);
			if((mux & 1023) == 5) lights[LRUN].setBrightness(0.f);
		}
		if(beats < len) {
			outputs[ORST].setVoltage(10.f);
			if((mux & 1023) == 6) lights[LRST].setBrightness(1.f);
		} else {
			outputs[ORST].setVoltage(0.f);
			if((mux & 1023) == 7) lights[LRST].setBrightness(0.f);
		}
		if(params[IS_RUN].getValue() > 0.5f) beatCounter += beatSamp;
		if(beats >= 64) {//sanity range before use
			beatCounter = modulo(beatCounter, 64);//beats long
		}
		//using reset overides clock in chained modules
	}
};

//geometry edit
#define HP 17
#define LANES 8
#define RUNGS 7

//placement macro
#define loc(x,y) mm2px(Vec(X_SPLIT*(1+2*(x-1)), (HEIGHT*Y_MARGIN)+Y_SPLIT*(1+2*(y-1))))

struct YWidget : ModuleWidget {
	YWidget(Y* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Y.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		for(int i = 0; i < 8; i++) {
			addParam(createParamCentered<LEDBezel>(loc(i + 1, 6), module, Y::QUADS + i));
			addParam(createParamCentered<LEDBezel>(loc(i + 1, 7), module, Y::QUADS + i + 8));

			addChild(createLightCentered<LEDBezelLight<GreenRedLight>>(loc(i + 1, 6), module, Y::LQUADS + i * 2));
			addChild(createLightCentered<LEDBezelLight<GreenRedLight>>(loc(i + 1, 7), module, Y::LQUADS + i * 2 + 16));

			addOutput(createOutputCentered<PJ301MPort>(loc(i + 1, 1), module, Y::OUTS + i));
			addOutput(createOutputCentered<PJ301MPort>(loc(i + 1, 1.75f), module, Y::OUTS + i + 8));
		}

		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 4; j++) {
				float x = i + 1.5f + (j % 2) * 4;
				float y = 6 - 0.5f + (j / 2);
				addParam(createParamCentered<LEDBezel>(loc(x, y), module, Y::TRIPS + i + j * 3));
				addChild(createLightCentered<LEDBezelLight<GreenRedLight>>(loc(x, y), module, Y::LTRIPS + i * 2 + j * 6));
			}
		}

		addParam(createParamCentered<LEDBezel>(loc(8, 4.75f), module, Y::RUN));
		addChild(createLightCentered<LEDBezelLight<GreenLight>>(loc(8, 4.75f), module, Y::LRUN));
		addOutput(createOutputCentered<PJ301MPort>(loc(8, 2.5f), module, Y::ORUN));

		addParam(createParamCentered<LEDBezel>(loc(7, 4.75f), module, Y::RST));
		addChild(createLightCentered<LEDBezelLight<GreenLight>>(loc(7, 4.75f), module, Y::LRST));
		addOutput(createOutputCentered<PJ301MPort>(loc(7, 2.5f), module, Y::ORST));

		addParam(createParamCentered<RoundBlackKnob>(loc(7.5f, 3.5f), module, Y::TEMPO));
		addParam(createParamCentered<RoundBlackKnob>(loc(1.5f, 3.5f), module, Y::LEN));

		addInput(createInputCentered<PJ301MPort>(loc(1, 2.5f), module, Y::ICV_BUT));
		addInput(createInputCentered<PJ301MPort>(loc(2, 2.5f), module, Y::IGATE_BUT));

		for(int j = 0; j < 4; j++) {
			float x = 3 + j;
			float y = 3.f;
			addParam(createParamCentered<LEDBezel>(loc(x, y), module, Y::MODES + j));
			addChild(createLightCentered<LEDBezelLight<GreenLight>>(loc(x, y), module, Y::LMODE + j));
		}

		addParam(createParamCentered<LEDBezel>(loc(1, 4.75f), module, Y::CPY));
		addChild(createLightCentered<LEDBezelLight<GreenLight>>(loc(1, 4.75f), module, Y::LCPY));

		addParam(createParamCentered<LEDBezel>(loc(2, 4.75f), module, Y::PST));
		addChild(createLightCentered<LEDBezelLight<GreenLight>>(loc(2, 4.75f), module, Y::LPST));

		addParam(createParamCentered<RoundBlackKnob>(loc(3.5f, 3.75f), module, Y::JAZZ3));
		addParam(createParamCentered<RoundBlackKnob>(loc(5.5f, 3.75f), module, Y::JAZZ4));
		addInput(createInputCentered<PJ301MPort>(loc(4.5f, 4.75f), module, Y::IPOS));
	}
};


Model* modelY = createModel<Y, YWidget>("Y");
