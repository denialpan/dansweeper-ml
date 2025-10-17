//
// Created by dern on 10/16/2025.
//

#include <dansweeperml/solver/ml/linearregression/linearregressiontrainer.h>
#include <filesystem>

#include "dansweeperml/core/render.h"
#include "dansweeperml/solver/ml/linearregression/features.h"

namespace mllinearregressiontrainer {

   LinearRegressionTrainer::LinearRegressionTrainer(int trainingRequired, std::string filePath) {
      this->trainEverySamples_ = trainingRequired;
      this->modelPath_ = filePath;
   }

   bool LinearRegressionTrainer::step(Grid::Grid& grid) {
      auto meta = grid.getMetadata();
      auto& cells = grid.getCells();

      // Pick a random unrevealed, unflagged cell
      std::vector<std::pair<int,int>> candidates;
      for (int y = 0; y < meta.height; ++y)
         for (int x = 0; x < meta.width; ++x)
            if (!cells[y][x].revealed && !cells[y][x].flagged)
               candidates.emplace_back(x, y);

      if (candidates.empty()) return false;

      std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
      auto [cx, cy] = candidates[dist(rng_)];

      // 1. Featurize before action
      std::vector<double> feat;
      features::featurize(grid, cx, cy, feat);

      // 2. Act: reveal
      grid.reveal(cx, cy);
      Render::queueHighlightTile(cx, cy);
      ++steps;

      // 3. Label after action (safe or mine)
      double label = 1.0;
      const auto metaAfter = grid.getMetadata();
      if (metaAfter.gridState == Grid::FINISHED_LOSE)
         label = 0.0;

      appendSample(feat, label);

      if (features_.n_cols >= trainEverySamples_) {
         trainAndSave();
      }

      return true;
   }

   void LinearRegressionTrainer::appendSample(const std::vector<double>& feature, double label) {
      const size_t D = feature.size();

      if (features_.n_rows == 0) {
         features_.set_size(D, 0);
         labels_.set_size(1, 0); // row vector
      } else if (features_.n_rows != D) {
         std::cerr << "feature mismatch (" << features_.n_rows << " : " << D << ")\n";
         return;
      }

      features_.insert_cols(features_.n_cols, 1);
      for (size_t i = 0; i < D; ++i)
         features_(i, features_.n_cols - 1) = feature[i];

      labels_.insert_cols(labels_.n_cols, 1);
      labels_(labels_.n_cols - 1) = label;
   }

   void LinearRegressionTrainer::trainAndSave() {

      if (features_.n_cols < 5000) {
         std::cout << "yeah howd you get here without enough examples???";
         return;
      }

      constexpr double lambda = 1e-4;

      lr_ = mlpack::LinearRegression<>(features_, labels_, lambda);

      std::filesystem::path path(modelPath_);
      std::filesystem::create_directories(path.parent_path());

      mlpack::data::Save(modelPath_, "linreg", lr_, false);
      std::cout << "saved model to " << modelPath_ << std::endl;

   }

   void LinearRegressionTrainer::reset() {
      steps = 0;
   }

   std::string LinearRegressionTrainer::getName() {
      return "linear regression trainer";
   }

   int LinearRegressionTrainer::getSteps() {
      return steps;
   }



} // mllinearregressiontrainer