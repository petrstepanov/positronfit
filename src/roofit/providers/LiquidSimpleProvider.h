/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParaProvider.h
 * Author: petrstepanov
 *
 * Created on December 6, 2018, 1:03 PM
 */

#ifndef PARAPROVIDER_H
#define PARAPROVIDER_H

#include "AbstractProvider.h"
#include "RooRealVar.h"

class LiquidSimpleProvider : public AbstractProvider{
public:
    LiquidSimpleProvider(RooRealVar* observable, RooRealVar* channelWidth);
    virtual ~LiquidSimpleProvider();

private:
    RooAbsPdf* initPdf(int i);
};

#endif /* PARAROVIDER_H */

