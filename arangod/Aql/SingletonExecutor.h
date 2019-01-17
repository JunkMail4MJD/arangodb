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

#ifndef ARANGOD_AQL_SINGLETON_EXECUTOR_H
#define ARANGOD_AQL_SINGLETON_EXECUTOR_H

#include "Aql/ExecutionState.h"
#include "Aql/ExecutorInfos.h"
#include "Aql/ConstFetcher.h"
#include "Aql/Stats.h"
#include "Aql/Variable.h"

#include "Aql/ExecutionBlockImpl.h"

namespace arangodb {
namespace transaction {
class Methods;
}

namespace aql {

class AllRowsFetcher;
class AqlItemMatrix;
class ExecutorInfos;
class NoStats;
class OutputAqlItemRow;
struct SortRegister;

class SingletonExecutor {
  template <typename T>
  friend class ExecutionBlockImpl;

 public:
  using Fetcher = ConstFetcher;
  using Infos = ExecutorInfos;
  using Stats = NoStats;

  SingletonExecutor(Fetcher& fetcher, ExecutorInfos&);
  ~SingletonExecutor();

  /**
   * @brief produce the next Row of Aql Values.
   *
   * @return ExecutionState,
   *         if something was written output.hasValue() == true
   */
  std::pair<ExecutionState, Stats> produceRow(OutputAqlItemRow& output);

 private:
  Infos& _infos;
  Fetcher& _fetcher;
  bool _done;
  std::unique_ptr<AqlItemBlock> _inputRegisterValues;
};
}  // namespace aql
}  // namespace arangodb

#endif
