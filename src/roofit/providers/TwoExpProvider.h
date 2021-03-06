/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TwoExpProvider.h
 * Author: petrstepanov
 *
 * Created on December 5, 2018, 8:43 AM
 */

#ifndef TWOEXPPROVIDER_H
#define TWOEXPPROVIDER_H

#include "AbstractProvider.h"
#include <RooRealVar.h>
#include <RooAbsPdf.h>
#include <RooArgSet.h>

class TwoExpProvider : public AbstractProvider {
public:
    TwoExpProvider(RooRealVar* observable, RooRealVar* channelWidth);
    virtual ~TwoExpProvider();

    static RooArgSet* getIndirectParameters(RooAbsPdf* pdf);

private:
    RooAbsPdf* initPdf(int i);

};

#endif /* EXPPROVIDER_H */

