/*
 * Thomas Keck 2017
 *
 * Simplified sklearn interface
 */

#pragma once

#include "FastBDT.h"
#include "FastBDT_IO.h"

#include <vector>

namespace FastBDT {
class Classifier {

  public:
      /*
       * Explicitly activate default/copy constructor and assign operator.
       * This was a request of a user.
       */
      Classifier() = default;
      Classifier(const Classifier&) = default;
      Classifier& operator=(const Classifier &) = default;

      Classifier(std::istream& stream) {

        stream >> m_version;
        stream >> m_nTrees;
        stream >> m_depth;
        stream >> m_binning;
        stream >> m_shrinkage;
        stream >> m_subsample;
        stream >> m_sPlot;
        stream >> m_flatnessLoss;
        stream >> m_purityTransformation;
        stream >> m_transform2probability;
        stream >> m_featureBinning;
        stream >> m_purityBinning;
        stream >> m_can_use_fast_forest;
        m_fast_forest = readForestFromStream<float>(stream);
        m_binned_forest = readForestFromStream<unsigned int>(stream);

      }

      friend std::ostream& operator<<(std::ostream& stream, const Classifier& classifier);

			Classifier(unsigned int nTrees, unsigned int depth, std::vector<unsigned int> binning, double shrinkage = 0.1, double subsample = 1.0, bool sPlot = false, double flatnessLoss = -1.0, std::vector<bool> purityTransformation = {}, unsigned int numberOfFlatnessFeatures=0, bool transform2probability=true) :
        m_nTrees(nTrees), m_depth(depth), m_binning(binning), m_shrinkage(shrinkage), m_subsample(subsample), m_sPlot(sPlot), m_flatnessLoss(flatnessLoss), m_purityTransformation(purityTransformation), m_numberOfFlatnessFeatures(numberOfFlatnessFeatures), m_transform2probability(transform2probability), m_can_use_fast_forest(true) {

          if(m_binning.size() <= m_numberOfFlatnessFeatures) {
            throw std::runtime_error("Number of provided binnings must be larger than the number of flat features, because there must be at least one ordinary feature in the training.");
          }

          if(m_purityTransformation.size() == 0) {
            for(unsigned int i = 0; i < m_binning.size() - m_numberOfFlatnessFeatures; ++i)
              m_purityTransformation.push_back(false);
          }

          for(auto p : m_purityTransformation)
            if(p)
              m_can_use_fast_forest = false;
      }
			
      void fit(const std::vector<std::vector<float>> &X, const std::vector<bool> &y, const std::vector<Weight> &w);

      float predict(const std::vector<float> &X) const;

  private:
    unsigned int m_version = 1;
    unsigned int m_nTrees;
    unsigned int m_depth;
    std::vector<unsigned int> m_binning;
    double m_shrinkage;
    double m_subsample;
    bool m_sPlot;
    double m_flatnessLoss;
    std::vector<bool> m_purityTransformation;
    unsigned int m_numberOfFlatnessFeatures;
    bool m_transform2probability;
    unsigned int m_numberOfFeatures;
    unsigned int m_numberOfFinalFeatures;
    std::vector<FeatureBinning<float>> m_featureBinning;
    std::vector<PurityTransformation> m_purityBinning;

    bool m_can_use_fast_forest;
    Forest<float> m_fast_forest;
    Forest<unsigned int> m_binned_forest;

};

std::ostream& operator<<(std::ostream& stream, const Classifier& classifier);

}