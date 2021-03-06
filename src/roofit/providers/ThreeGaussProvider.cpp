/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreeGaussProvider.cpp
 * Author: petrstepanov
 * 
 * Created on December 6, 2018, 10:48 AM
 */

#include "ThreeGaussProvider.h"
#include "RooFormulaVar.h"
#include "../pdfs/ThreeGaussian.h"
#include "../../roofit/AdditiveConvolutionPdf.h"
#include "../../model/Constants.h"
#include "../../util/RootHelper.h"

ThreeGaussProvider::ThreeGaussProvider(RooRealVar* _observable, RooRealVar* _channelWidth) : AbstractProvider(_observable, _channelWidth) {}

ThreeGaussProvider::~ThreeGaussProvider() {
}

RooAbsPdf* ThreeGaussProvider::initPdf(int i) {
	RooConstVar* fwhm2disp = Constants::fwhm2disp;

    // 1st Gauss FWHM
    RooRealVar* g1FWHM = new RooRealVar("FWHM_gauss1", "1st gauss FWHM", 0.3, 0.1, 0.4, "ns");
    RooFormulaVar* g1Dispersion = new RooFormulaVar("gauss1Dispersion", "@0*@1/@2", RooArgList(*g1FWHM, *fwhm2disp, *channelWidth));

    // 2nd gauss FWHM
    RooRealVar* g2FWHM = new RooRealVar("FWHM_gauss2", "2nd gauss FWHM", 0.8, 0.4, 1.5, "ns");
    RooFormulaVar* g2Dispersion = new RooFormulaVar("gauss2Dispersion", "@0*@1/@2", RooArgList(*g2FWHM, *fwhm2disp, *channelWidth));

    // 3nd gauss FWHM
    RooRealVar* g3FWHM = new RooRealVar("FWHM_gauss3", "3rd gauss FWHM", 1.5, 0.8, 5.0, "ns");
	RooFormulaVar* g3Dispersion = new RooFormulaVar("gauss3Dispersion", "@0*@1/@2", RooArgList(*g3FWHM, *fwhm2disp, *channelWidth));

    // Fraction of the 2nd gauss
    RooRealVar* g2Fraction = new RooRealVar("Int_gauss2", "2nd gauss fraction", 1, 0, 50, "%");
    RooFormulaVar* g2FractionNorm = new RooFormulaVar("gauss2FracNorm", "@0/100", *g2Fraction);

	// Fraction of the 3rd gauss
	RooRealVar* g3Fraction = new RooRealVar("Int_gauss3", "3rd gauss fraction", 1, 0, 10, "%");
	RooFormulaVar* g3FractionNorm = new RooFormulaVar("gauss3FracNorm", "@0/100", *g3Fraction);

	// Zero channel start values are assigned later (relative to the MIN_CHANNEL value)
    RooRealVar* gMean = new RooRealVar(AdditiveConvolutionPdf::VAR_MEAN_GAUSS_NAME, "Resolution function mean", 1, "ch");

	return new ThreeGaussian("pdfResolution3Gauss", "Resolution function", *observable, *gMean, *g1Dispersion, *g3Dispersion, *g2Dispersion, *g3FractionNorm, *g2FractionNorm);
}

RooArgSet* ThreeGaussProvider::getIndirectParameters(RooAbsPdf* pdf){
    RooArgSet* indirectParameters = new RooArgSet();

    // pdf parameters might have suffixed names so we account on that
    // for "Int_exp2" we pull "Int_exp2_##" as well
    RooRealVar* g2Fraction = RootHelper::getParameterByNameCommonOrLocal(pdf, "Int_gauss2");
    RooRealVar* g3Fraction = RootHelper::getParameterByNameCommonOrLocal(pdf, "Int_gauss3");

    if (g2Fraction){
    	RooFormulaVar* g1Fraction = new RooFormulaVar("Int_gauss1", "100-@0-@1", RooArgList(*g2Fraction, *g3Fraction));
    	indirectParameters->add(*g1Fraction);
    }
    return indirectParameters;
}
