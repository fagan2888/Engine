/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

/*! \file portfolio/builders/fxoption.hpp
    \brief
    \ingroup builders
*/

#pragma once

#include <boost/make_shared.hpp>
#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>

namespace ore {
namespace data {

//! Engine Builder for European FX Options
/*! Pricing engines are cached by currency pair

    \ingroup builders
 */
class FxOptionEngineBuilder : public CachingPricingEngineBuilder<string, const Currency&, const Currency&> {
public:
    FxOptionEngineBuilder() : CachingEngineBuilder("GarmanKohlhagen", "AnalyticEuropeanEngine", {"FxOption"}) {}

protected:
    virtual string keyImpl(const Currency& forCcy, const Currency& domCcy) override {
        return forCcy.code() + domCcy.code();
    }

    virtual boost::shared_ptr<PricingEngine> engineImpl(const Currency& forCcy, const Currency& domCcy) override {
        string pair = keyImpl(forCcy, domCcy);
        boost::shared_ptr<GeneralizedBlackScholesProcess> gbsp = boost::make_shared<GeneralizedBlackScholesProcess>(
            market_->fxSpot(pair, configuration(MarketContext::pricing)),
            market_->discountCurve(forCcy.code(),
                                   configuration(MarketContext::pricing)), // dividend yield ~ foreign yield
            market_->discountCurve(domCcy.code(), configuration(MarketContext::pricing)),
            market_->fxVol(pair, configuration(MarketContext::pricing)));
        return boost::make_shared<AnalyticEuropeanEngine>(gbsp);
    }
};

} // namespace data
} // namespace ore
