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
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#include "IdExecutor.h"
#include "Aql/AqlValue.h"
#include "Aql/OutputAqlItemRow.h"
#include "Basics/Common.h"

#include <algorithm>

using namespace arangodb;
using namespace arangodb::aql;

IdExecutorInfos::IdExecutorInfos(RegisterId nrInOutRegisters,
                                 std::unordered_set<RegisterId> whiteList,
                                 std::unordered_set<RegisterId> whiteListClean,
                                 std::unordered_set<RegisterId> registersToClear)
    : ExecutorInfos(make_shared_unordered_set(), make_shared_unordered_set(),
                    nrInOutRegisters, nrInOutRegisters,
                    std::move(registersToClear), std::move(whiteListClean)),
      _whiteList(std::move(whiteList)) {}

IdExecutor::IdExecutor(Fetcher& fetcher, IdExecutorInfos& infos)
    : _infos(infos), _fetcher(fetcher){};
IdExecutor::~IdExecutor() = default;

std::pair<ExecutionState, IdExecutor::Stats> IdExecutor::produceRow(OutputAqlItemRow& output) {
  ExecutionState state;
  IdExecutor::Stats stats;
  bla++;

  InputAqlItemRow inputRow = InputAqlItemRow{CreateInvalidInputRowHint{}};
  std::tie(state, inputRow) = _fetcher.fetchRow();

  if (state == ExecutionState::WAITING) {
    TRI_ASSERT(!inputRow);
    LOG_DEVEL << this << " WAITING " << bla;
    return {state, std::move(stats)};
  }

  if (!inputRow) {
    TRI_ASSERT(state == ExecutionState::DONE);
    LOG_DEVEL << this << " DONE " << bla;
    return {state, std::move(stats)};
  }

  TRI_ASSERT(state == ExecutionState::HASMORE || state == ExecutionState::DONE);
  output.copyRow(inputRow);

  LOG_DEVEL_IF(state == ExecutionState::DONE) << this << " DONE " << bla;
  LOG_DEVEL_IF(state == ExecutionState::HASMORE) << this << " HASMORE " << bla;

  return {state, std::move(stats)};
}
