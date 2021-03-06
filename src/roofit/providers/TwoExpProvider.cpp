/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TwoExpProvider.cpp
 * Author: petrstepanov
 * 
 * Created on December 5, 2018, 8:43 AM
 */

#include "TwoExpProvider.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "../pdfs/TwoExpPdf.h"
#include "../../model/Constants.h"
#include "../../util/RootHelper.h"
#include <iostream>

TwoExpProvider::TwoExpProvider(RooRealVar* _observable, RooRealVar* _channelWidth) : AbstractProvider(_observable, _channelWidth) {}

TwoExpProvider::~TwoExpProvider() {
}

RooAbsPdf* TwoExpProvider::initPdf(int i) {
//    RooConstVar* channelWidth = Constants::getInstance()->getRooChannelWidth();
    
    // Instantiate RooRealVar parameters
    RooRealVar* tau1 = new RooRealVar("#tau1", "1st positron lifetime", 0.2, 0.1, 2, "ns");
    RooFormulaVar* tau1Ch = new RooFormulaVar("tau1Ch", "1st positron lifetime, channels", "@0/@1", RooArgList(*tau1, *channelWidth));

    RooRealVar* tau2 = new RooRealVar("#tau2", "2nd positron lifetime", 0.5, 0.3, 10, "ns");
    RooFormulaVar* tau2Ch = new RooFormulaVar("tau2Ch", "2nd positron lifetime, channels", "@0/@1", RooArgList(*tau2, *channelWidth));

    RooRealVar* Int_exp2 = new RooRealVar("Int_exp2", "2nd exponent intensity", 20, 0, 100, "%");
    RooFormulaVar* intExp2Norm = new RooFormulaVar("intExp2Norm", "@0/100", *Int_exp2);

    // Instantinate model
    return new TwoExpPdf("twoExp", "Two exponential decay", *observable, *tau1Ch, *tau2Ch, *intExp2Norm);
}

RooArgSet* TwoExpProvider::getIndirectParameters(RooAbsPdf* pdf){
    RooArgSet* indirectParameters = new RooArgSet();

    // pdf parameters might have suffixed names so we account on that
    // for "Int_exp2" we pull "Int_exp2_##" as well
    RooRealVar* Int_exp2 = RootHelper::getParameterByNameCommonOrLocal(pdf, "Int_exp2");
    if (Int_exp2){
    	RooFormulaVar* Int_exp1 = new RooFormulaVar("Int_exp1", "100-@0", RooArgList(*Int_exp2));
    	indirectParameters->add(*Int_exp1);
    }
    return indirectParameters;
}
