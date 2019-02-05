/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphicsHelper.cpp
 * Author: petrstepanov
 * 
 * Created on December 5, 2018, 10:12 AM
 */

#include "GraphicsHelper.h"
#include "StringUtils.h"
#include <TBox.h>
#include <TPaveText.h>
#include <RooRealVar.h>
#include <iostream>

const Double_t GraphicsHelper::FONT_SIZE_SMALL = 0.028;
const Double_t GraphicsHelper::FONT_SIZE_NORMAL = 0.05;
const Double_t GraphicsHelper::RESIDUALS_PAD_RELATIVE_HEIGHT = 0.35;
const Double_t GraphicsHelper::LEGEND_XMIN = 0.7;

Double_t GraphicsHelper::getSpectrumPadFontFactor(){
	return 0.5/(1-RESIDUALS_PAD_RELATIVE_HEIGHT);
}

Double_t GraphicsHelper::getResidualsPadFontFactor(){
	// Pad's font size depends on its height. Uneven pad heights result in different font sizes.
	// This function fives a coefficient that evens the font size out
	return 0.5/RESIDUALS_PAD_RELATIVE_HEIGHT;
}


void GraphicsHelper::drawRegion(RooPlot* frame, Int_t xMin, Int_t xMax) {
	Double_t yMin = frame->GetMinimum(); // frame->GetYaxis()->GetXmin();
	Double_t yMax = frame->GetMaximum(); // frame->GetYaxis()->GetXmax();
	TBox* sBox = new TBox(xMin, yMin, xMax, yMax);
	sBox->SetLineWidth(0);
	sBox->SetFillColorAlpha(15, 0.2);
	frame->addObject(sBox);

	std::cout << "xMin: " << xMin << ", xMax: " << xMax << std::endl;
	std::cout << "yMin: " << yMin << ", yMax: " << yMax << std::endl;
}

void GraphicsHelper::printVariable(Int_t sigDigits, const char* options, Int_t& currentLine, RooRealVar* var, TPaveText* box, RooArgList* paramsList) {
	TString* formatted = var->format(sigDigits, options);
	TText* t = box->AddText(formatted->Data());
	Bool_t notConstant = !var->isConstant();
	Bool_t atMaximum = var->getVal() + var->getError() > var->getMax();
	Bool_t atMinimum = var->getVal() - var->getError() < var->getMin();
	if (notConstant && (atMaximum || atMinimum)) t->SetTextColor(kPink-8);
	paramsList->remove(*var);
	currentLine++;
}

TPaveText* GraphicsHelper::makePaveText(const RooArgSet& params, Double_t xmin, Double_t xmax, Double_t ymax) {

	Bool_t showConstants = kTRUE;
	Bool_t showLabel = kFALSE;

	// parse the options
//	TString opts = TString("NEU");
//	opts.ToLower();

	const char* options = "NEULP";


	// calculate the box's size, adjusting for constant parameters
	TIterator* pIter = params.createIterator();

	Double_t ymin(ymax), dy(0.03);
	RooRealVar *var = 0;
	while ((var = (RooRealVar*) pIter->Next())) {
		if (showConstants || !var->isConstant())
			ymin -= dy;
	}

	if (showLabel)
		ymin -= dy;

	ymin -= dy*5; // New line on top abd bottom plus three lines

	// create the box and set its options
	TPaveText *box = new TPaveText(xmin, ymax, xmax, ymin, "BRNDC");
	if (!box)
		return 0;
	box->SetName("myParamBox");
	box->SetFillColor(0);
	box->SetBorderSize(1);
	box->SetTextAlign(12);
	box->SetTextSize(FONT_SIZE_SMALL);
	box->SetFillStyle(1001);
	box->SetFillColorAlpha(0, 0.9);
	//char buffer[512];

	RooArgList* paramsList = new RooArgList(params);
	paramsList->sort();

	// Line counter (to draw horizontal lines later)
	Int_t currentLine = 0;
	std::vector<int> hrLineNumbers;

	box->AddText("");
	currentLine++;

	Int_t sigDigits = 0;
	if (RooRealVar* var = findRooRealVarInList(paramsList, "bins")) {
		printVariable(sigDigits, options, currentLine, var, box, paramsList);
	}

	if (RooRealVar* var = findRooRealVarInList(paramsList, "integral")) {
		printVariable(sigDigits, options, currentLine, var, box, paramsList);
	}

	sigDigits = 3;
	if (RooRealVar* var = findRooRealVarInList(paramsList, "background")) {
		printVariable(sigDigits, options, currentLine, var, box, paramsList);
	}

	hrLineNumbers.push_back(currentLine);
	box->AddText("");
	currentLine++;

	while (RooRealVar* var = findRooRealVarInList(paramsList, "ource")) {
		printVariable(sigDigits, options, currentLine, var, box, paramsList);
	}

	hrLineNumbers.push_back(currentLine);
	box->AddText("");
	currentLine++;

	while (RooRealVar* var = findRooRealVarInList(paramsList, "gauss")) {
		printVariable(sigDigits, options, currentLine, var, box, paramsList);
	}

	hrLineNumbers.push_back(currentLine);
	box->AddText("");
	currentLine++;

	//	box->AddLine(0, 0.1, 1, 0.1);

//	pIter->Reset();
//	while ((var = (RooRealVar*) pIter->Next())) {
//		if (var->isConstant() && !showConstants)
//			continue;
//
//		TString *formatted = options ? var->format(sigDigits, options) : var->format(*formatCmd);
//		box->AddText(formatted->Data());
//		delete formatted;
//	}

	pIter = paramsList->createIterator();
	while ((var = (RooRealVar*) pIter->Next())) {
		if (var->isConstant() && !showConstants) continue;
		TString *formatted = var->format(sigDigits, options);
		box->AddText(formatted->Data());
		currentLine++;
	}

	box->AddText("");
	currentLine++;

	// Draw horizontal lines
	std::cout << "GraphicsHelper::makePaveText" << std::endl;
	for (std::vector<int>::iterator it = hrLineNumbers.begin(); it != hrLineNumbers.end(); ++it){
		std::cout << "line at: " << *it << "; total: " << currentLine << std::endl;
		Double_t y = ((Double_t)*it+0.5)/(Double_t)currentLine;
		box->AddLine(0, 1-y, 1, 1-y);
	}


	// add the optional label if specified
	if (showLabel)
		box->AddText("label");

	// Add box to frame
//	frame->addObject(box);

	delete pIter;
	return box;
}

// Finds RooRealVar in list by a name substring
RooRealVar* GraphicsHelper::findRooRealVarInList(RooArgList* list, const char* nameSubstring) {
	TIterator* it = list->createIterator();
	TObject* temp;
	while ((temp = it->Next())) {
		if (RooRealVar* rooRealVar = dynamic_cast<RooRealVar*>(temp)) {
			const char* name = rooRealVar->GetName();
			if (StringUtils::isSubstring(name, nameSubstring)) {
				return rooRealVar;
			}
		}
	}
	return NULL;
}
