/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RootUtils.cpp
 * Author: petrstepanov
 *
 * Created on December 4, 2018, 2:09 PM
 */

#include "RootHelper.h"
#include "StringUtils.h"
#include "Debug.h"
#include <TUnixSystem.h>
#include <RooWorkspace.h>
#include <TObjString.h>
#include <TMatrixDUtils.h>
#include <RooHist.h>
#include <RooCurve.h>
#include <iostream>
#include <utility>

//void RootHelper::deleteObject(const char* name){
//    TObject* obj = gROOT->FindObject(name);
//    deleteObject(obj);
//}
//
//void RootHelper::deleteObject(TObject* obj){
//    if (obj){
//        obj->Delete();
//    }
//}

Int_t RootHelper::getNumCpu() {
	SysInfo_t sysInfo;
	gSystem->GetSysInfo(&sysInfo);
	std::cout << "RootHelper::getNumCpu: sysInfo.fCpus = " << sysInfo.fCpus << std::endl;
	return (sysInfo.fCpus >= 0) ? sysInfo.fCpus : 1;
}

void RootHelper::printPdfCoefficientNames(RooAbsPdf* pdf, RooRealVar* observable) {
	std::cout << pdf->GetName() << ": ";

	RooArgSet* params = pdf->getParameters(*observable);
	TIterator* it = params->createIterator();
	TObject* temp;
	while ((temp = it->Next())) {
		TNamed* named = dynamic_cast<TNamed*>(temp);
		if (named) {
			std::cout << named->GetName() << " ";
		}
	}
	std::cout << std::endl;
}

//RooRealVar* RootHelper::findParameterNameContains(RooAbsPdf* pdf, RooRealVar* observable, const char* substring) {
//	RooArgSet* params = pdf->getParameters(*observable);
//	TIterator* it = params->createIterator();
//	TObject* temp;
//	while ((temp = it->Next())) {
//		if (RooRealVar* var = dynamic_cast<RooRealVar*>(temp)) {
//			if (StringUtils::isSubstring(var->GetName(), substring)){
//				return var;
//			}
//		}
//	}
//	return NULL;
//}

RooAbsPdf* RootHelper::suffixPdfAndNodes(RooAbsPdf* pdf, RooRealVar* observable, const char* suffix){
	#ifdef USEDEBUG
		std::cout << "RootHelper::suffixPdfAndNodes" << std::endl;
	#endif

	RooWorkspace* w = new RooWorkspace("w", "w");
	w->import(*pdf, RooFit::RenameAllVariablesExcept(suffix, observable->GetName()), RooFit::RenameAllNodes(suffix));
	#ifdef USEDEBUG
		w->Print();
	#endif
	TString suffixedName = TString::Format("%s_%s", pdf->GetName(), suffix);
	RooAbsPdf* suffixedPdf =  w->pdf(suffixedName.Data());
	return suffixedPdf;
}

void RootHelper::setRooRealVarValueLimits(RooRealVar* var, Double_t value, Double_t min, Double_t max){
	// Tip: roofit can't set min larger than current max and vice versa
	var->setMin(std::numeric_limits<double>::min());
	var->setMax(std::numeric_limits<double>::max());
	var->setMin(min);
	var->setMax(max);
	var->setVal(value);
}

RooRealVar* RootHelper::getParameterByNameCommonOrLocal(RooAbsPdf* pdf, const char* name){
	// for given parameter name, say #tau1 will return either exact match or suffixed:
	// #tau1 or #tau1_13 or #tau1_2
	std::string nameString(name);

	// TODO: check list is not empty otherwise pass observable
	RooArgSet* parameters = pdf->getParameters(RooArgSet()); // pdf->getParameters(*observable);
	TIterator* it = parameters->createIterator();
	while (TObject* temp = it->Next()) {
		if (RooRealVar* parameter = dynamic_cast<RooRealVar*>(temp)) {
			std::string parameterName = parameter->GetName();
			std::string parameterNameWithoutSuffix = StringUtils::getStringWithoutSuffix(parameterName);
			if (nameString == parameterNameWithoutSuffix){
				return parameter;
			}
		}
	}
	return NULL;
}

RooAbsPdf* RootHelper::getComponentByNameCommonOrLocal(RooAbsPdf* pdf, const char* name){
	// for given parameter name, say #tau1 will return either exact match or suffixed:
	// #tau1 or #tau1_13 or #tau1_2
	std::string nameString(name);

	RooArgSet* components = pdf->getComponents();
	TIterator* it = components->createIterator();
	while (TObject* temp = it->Next()) {
		if (RooAbsPdf* component = dynamic_cast<RooAbsPdf*>(temp)) {
			std::string componentName = component->GetName();
			std::string componentNameWithoutSuffix = StringUtils::getStringWithoutSuffix(componentName);
			if (nameString == componentNameWithoutSuffix){
				return component;
			}
		}
	}
	return NULL;
}

// Deprecated, use getParameterByNameCommonOrLocal()
RooRealVar* RootHelper::getParameterNameContains(RooAbsPdf* pdf, const char* nameSubstring){
	RooArgSet* parameters = pdf->getParameters(RooArgSet());
	TIterator* it = parameters->createIterator();
	while (TObject* temp = it->Next()) {
		if (RooRealVar* rrv = dynamic_cast<RooRealVar*>(temp)) {
			if (StringUtils::isSubstring(rrv->GetName(), nameSubstring)){
				return rrv;
			}
		}
	}
	return nullptr;
}

RooAbsPdf* RootHelper::getComponentNameContains(RooAbsPdf* pdf, const char* nameSubstring){
	RooArgSet* components = pdf->getComponents();
	TIterator* it = components->createIterator();
	while (TObject* temp = it->Next()) {
		if (RooAbsPdf* rap = dynamic_cast<RooAbsPdf*>(temp)) {
			if (StringUtils::isSubstring(rap->GetName(), nameSubstring)){
				return rap;
			}
		}
	}
	return nullptr;
}

// Finds RooRealVar in list by a name substring
RooAbsArg* RootHelper::findArgNameSubstring(RooAbsCollection* list, const char* nameSubstring) {
	TIterator* it = list->createIterator();
	while (TObject* temp = it->Next()) {
		if (RooAbsArg* rooAbsArg = dynamic_cast<RooAbsArg*>(temp)) {
			const char* name = rooAbsArg->GetName();
			if (StringUtils::isSubstring(name, nameSubstring)) {
				return rooAbsArg;
			}
		}
	}
	return NULL;
}

// Finds RooRealVar in list by a name substring and with certain attribute value
RooArgList* RootHelper::findArgsNameSubstring(RooAbsCollection* list, const char* nameSubstring) {
	RooArgList* argsList = new RooArgList();
	TIterator* it = list->createIterator();
	while (TObject* temp = it->Next()) {
		if (RooAbsArg* rooAbsArg = dynamic_cast<RooAbsArg*>(temp)) {
			const char* name = rooAbsArg->GetName();
			if (StringUtils::isSubstring(name, nameSubstring)) {
				argsList->add(*rooAbsArg);
			}
		}
	}
	return argsList;
}


// Makes data points - a pair of Matrix (ascii columns) with column captions TList (TString)
std::pair<TMatrixD,TList*> RootHelper::rooPlotToMatrix(RooRealVar* axis, RooPlot* plot, Double_t conversionValue, Double_t zeroChannel, const char* conversionUnit){
	// Calculate number of columns in matrix
	Int_t numberOfColumns = 1; // originally we have one column just for channels

	if (conversionValue != 0) numberOfColumns++;  // add column for channels converted into say nanoseconds

	for (int i=0; i < plot->numItems(); i++){
		TObject* object = plot->getObject(i);
		if (RooHist* hist = dynamic_cast<RooHist*>(object)){
			numberOfColumns+=2; // RooHist has value and error column
		}
		if (RooCurve* curve = dynamic_cast<RooCurve*>(object)){
			numberOfColumns++; // RooCurve has only value column
		}
	}

	// Calculate number of rows in matrix (just number of axis bins)
	const RooAbsBinning* binning = &(axis->getBinning());
	Int_t numberOfRows = binning->numBins();

	// Start filling matrix with data columns
	TMatrixD matrix(numberOfRows, numberOfColumns);
	TList* columnNames = new TList();
	Int_t currentColumn = 0;

	// Fill first column with channel center bin coordinates
	columnNames->Add(new TObjString("channel"));
	TMatrixDColumn column(matrix, currentColumn); // get first column
	for (int i = 0; i < numberOfRows; i++){
		column(i) = (Double_t)binning->binCenter(i);
	}
	currentColumn++;

	// Next second column with channels converted to something  (e.g. nanoseconds)
	if (conversionValue !=0){
		columnNames->Add(new TObjString(conversionUnit));
		TMatrixDColumn column(matrix, currentColumn); // get first column
		for (int i = 0; i < numberOfRows; i++){
			column(i) = ((Double_t)binning->binCenter(i) - zeroChannel) * conversionValue;
		}
		currentColumn++;
	}

	// Iterate through RooHists and RooCurves in the list of RooPlots
	for (int p=0; p < plot->numItems(); p++){
		TObject* object = plot->getObject(p);
		if (RooHist* hist = dynamic_cast<RooHist*>(object)){
			TString columnValueName = TString::Format("%s_value", hist->GetName());
			columnNames->Add(new TObjString(columnValueName.Data()));
			TString columnErrorName = TString::Format("%s_error", hist->GetName());
			columnNames->Add(new TObjString(columnErrorName.Data()));
			TMatrixDColumn columnValue(matrix, currentColumn);
			TMatrixDColumn columnError(matrix, currentColumn+1);
			for (int i = 0; i < numberOfRows; i++){
				Double_t binCenter = binning->binCenter(i);
				columnValue(i) = hist->Eval(binCenter);
				columnError(i) = hist->GetErrorY(i);
			}
			currentColumn+=2;
		}
		if (RooCurve* curve = dynamic_cast<RooCurve*>(object)){
			columnNames->Add(new TObjString(curve->GetName()));
			TMatrixDColumn column(matrix, currentColumn);
			for (int i = 0; i < numberOfRows; i++){
				Double_t binCenter = binning->binCenter(i);
				column(i) = curve->Eval(binCenter);
			}
			currentColumn++;
		}
	}

	// Print matrix
	#ifdef USEDEBUG
		std::cout << "RootHelper::rooPlotToMatrix" << std::endl;
//		columnNames->Print();
//		matrix.Print();
	#endif

	return std::make_pair(matrix,columnNames);
}

