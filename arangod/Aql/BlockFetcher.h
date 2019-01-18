////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2018 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Tobias Gödderz
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_BLOCK_FETCHER_H
#define ARANGOD_AQL_BLOCK_FETCHER_H

#include "Aql/AqlItemBlock.h"
#include "Aql/AqlItemBlockShell.h"
#include "Aql/ExecutionBlock.h"
#include "Aql/ExecutionEngine.h"
#include "Aql/ExecutionState.h"
#include "Basics/Exceptions.h"

#include <memory>
#include <utility>

namespace arangodb {
namespace aql {

/**
 * @brief Thin interface to access the methods of ExecutionBlock that are
 * necessary for the row Fetchers. Makes it easier to test the Fetchers.
 */
class BlockFetcher {
 public:
  /**
   * @brief Interface to fetch AqlItemBlocks from upstream with getSome that
   *        wraps them into InputAqlItemBlockShells.
   * @param dependencies Dependencies of the current ExecutionBlock. Must
   *                     contain EXACTLY ONE element. Otherwise, BlockFetcher
   *                     may be instantiated, but never used. It is allowed to
   *                     pass a reference to an empty vector, but as soon as
   *                     the BlockFetcher is used, the condition must be
   *                     satisfied.
   * @param itemBlockManager All blocks fetched via dependencies[0]->getSome()
   *                         will later be returned to this AqlItemBlockManager.
   * @param inputRegisters Set of registers the current ExecutionBlock is
   *                       allowed to read.
   * @param nrInputRegisters Total number of registers of the AqlItemBlocks
   *                         here. Called nrInputRegisters to discern between
   *                         the widths of input and output blocks.
   *
   * The constructor MAY NOT access the dependencies, nor the itemBlockManager.
   * This is because the dependencies will be added to the ExecutionBlock only
   * after construction, and to allow derived subclasses for testing (read
   * BlockFetcherMock) to create them *after* the parent class was constructed.
   */
  BlockFetcher(
      std::vector<ExecutionBlock*> const& dependencies,
      AqlItemBlockManager& itemBlockManager,
      std::shared_ptr<const std::unordered_set<RegisterId>> inputRegisters,
      RegisterId nrInputRegisters)
      : _dependencies(dependencies),
        _itemBlockManager(itemBlockManager),
        _inputRegisters(std::move(inputRegisters)),
        _nrInputRegisters(nrInputRegisters) {}

  TEST_VIRTUAL ~BlockFetcher() = default;

  TEST_VIRTUAL
  std::pair<ExecutionState, std::shared_ptr<InputAqlItemBlockShell>>
  fetchBlock();

  TEST_VIRTUAL inline RegisterId getNrInputRegisters() const {
    return _nrInputRegisters;
  }

 protected:
  AqlItemBlockManager& itemBlockManager() { return _itemBlockManager; }
  AqlItemBlockManager const& itemBlockManager() const {
    return _itemBlockManager;
  }

  ExecutionBlock& upstreamBlock() {
    TRI_ASSERT(_dependencies.size() == 1);
    return *_dependencies[0];
  }

 private:
  std::vector<ExecutionBlock*> const& _dependencies;
  AqlItemBlockManager& _itemBlockManager;
  std::shared_ptr<const std::unordered_set<RegisterId>> const _inputRegisters;
  RegisterId const _nrInputRegisters;
};

}  // namespace aql
}  // namespace arangodb

#endif  // ARANGOD_AQL_BLOCK_FETCHER_H
