//
// Created by dern on 10/16/2025.
//

#ifndef DANSWEEPER_ML_LINEARREGRESSIONTRAINER_H
#define DANSWEEPER_ML_LINEARREGRESSIONTRAINER_H
#include "dansweeperml/core/grid.h"
#include "dansweeperml/solver/isolver.h"
#include "mlpack/core.hpp"
#include "mlpack/methods/linear_regression/linear_regression.hpp"

namespace mllinearregressiontrainer {
    class LinearRegressionTrainer : public ISolver {

    public:
        explicit LinearRegressionTrainer(int trainEverySamples = 5000, std::string filePath = "models/defaultlr.bin");

        bool step(Grid::Grid& grid) override;
        std::string getName() override;
        int getSteps() override;
        void reset() override;

    protected:
        std::string name = "linear regression trainer";

    private:
        void appendSample(const std::vector<double>& feature, double label);
        void trainAndSave();
        void saveModel();

        std::string modelPath_;
        int trainEverySamples_;

        arma::mat features_;
        arma::rowvec labels_;
        mlpack::LinearRegression<> lr_;

        std::mt19937 rng_{std::random_device{}()};
    };
} // mllinearregressiontrainer

#endif //DANSWEEPER_ML_LINEARREGRESSIONTRAINER_H