/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GrainPdf.cpp
 * Author: petrstepanov
 * 
 * Created on November 9, 2016, 8:48 PM
 */

#include "GrainPdf.h"

GrainPdf::GrainPdf(const char *name, const char *title,
    RooAbsReal& _t,
    RooAbsReal& _lambdaJ,
    RooAbsReal& _lambdaBulk,
    RooAbsReal& _lambdaGrain,
    RooAbsReal& _lambdaVac,
    RooAbsReal& _kappaVac
    ) :
    RooAbsPdf(name, title),
    t("t", "t", this, _t),
    lambdaJ("lambdaJ", "lambdaJ", this, _lambdaJ),
    lambdaBulk("lambdaBulk", "lambdaBulk", this, _lambdaBulk),
    lambdaGrain("lambdaGrain", "lambdaGrain", this, _lambdaGrain),
    lambdaVac("lambdaVac", "lambdaVac", this, _lambdaVac),
    kappaVac("kappaVac", "kappaVac", this, _kappaVac){
}

GrainPdf::GrainPdf(const GrainPdf& other, const char* name) :
RooAbsPdf(other, name),
t("t", this, other.t),
lambdaJ("lambdaJ", this, other.lambdaJ),
lambdaBulk("lambdaBulk", this, other.lambdaBulk),
lambdaGrain("lambdaGrain", this, other.lambdaGrain),
lambdaVac("lambdaVac", this, other.lambdaVac),
kappaVac("kappaVac", this, other.kappaVac){
}

Double_t GrainPdf::evaluate() const {   
    if (t < 0) return 0.;
    Double_t iterations = 5;
    Double_t piSquare = TMath::Pi()*TMath::Pi();
    
    // n_b
    Double_t n_b = 0;
    for (unsigned n = 1; n <= iterations; n++){
        n_b += exp((-1.)*lambdaJ*n*n*t) / (n*n);
    }
    n_b = 6 * exp((-1.)*(lambdaBulk + kappaVac)*t) / piSquare * n_b;

    // n_v
    Double_t n_v = 0;
    for (unsigned n = 1; n <= iterations; n++){
        n_v += (exp((-1.)*lambdaVac*t) - exp((-1.)*(lambdaBulk + kappaVac + lambdaJ*n*n)*t)) / (n*n*(lambdaBulk + kappaVac + lambdaJ*n*n - lambdaVac));
    }
    n_v = 6 * kappaVac / piSquare * n_v;

    // n_g
    Double_t n_g = 0;
    for (unsigned n = 1; n <= iterations; n++){
        n_g += (exp((-1.)*lambdaGrain*t) - exp((-1.)*(lambdaBulk + kappaVac + lambdaJ*n*n)*t)) / (lambdaBulk + kappaVac + lambdaJ*n*n - lambdaGrain);
    }
    n_g = 6 * lambdaJ / piSquare * n_g;

    return lambdaBulk*n_b + lambdaVac*n_v + lambdaGrain*n_g;
}