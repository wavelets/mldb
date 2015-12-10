#pragma once

#include "mldb/server/dataset.h"
#include "mldb/server/procedure.h"
#include "mldb/server/algorithm.h"
#include "mldb/server/function.h"
#include "matrix.h"
#include "mldb/ml/value_descriptions.h"
#include "metric_space.h"

 #include <boost/numeric/ublas/vector.hpp>
 #include <boost/numeric/ublas/vector_proxy.hpp>
 #include <boost/numeric/ublas/matrix.hpp>
 #include <boost/numeric/ublas/triangular.hpp>
 #include <boost/numeric/ublas/lu.hpp>
 #include <boost/numeric/ublas/io.hpp>

namespace Datacratic {
namespace MLDB {

struct EMConfig {
    EMConfig()
        : select("*"),
          where(SqlExpression::parse("true")),
          numInputDimensions(-1),
          numClusters(10),
          maxIterations(100)
    {
    }

    std::shared_ptr<TableExpression> dataset;
    PolyConfigT<Dataset> output;
    PolyConfigT<Dataset> centroids;
    SelectExpression select;
    std::shared_ptr<SqlExpression> where;

    int numInputDimensions;
    int numClusters;
    int maxIterations;
};

DECLARE_STRUCTURE_DESCRIPTION(EMConfig);



/*****************************************************************************/
/* EM PROCEDURE                                                           */
/*****************************************************************************/

struct EMProcedure: public Procedure {
    
    EMProcedure(MldbServer * owner,
                PolyConfig config,
                const std::function<bool (const Json::Value &)> & onProgress);

    virtual RunOutput run(const ProcedureRunConfig & run,
                          const std::function<bool (const Json::Value &)> & onProgress) const;

    virtual Any getStatus() const;

    EMConfig emConfig;
};


/*****************************************************************************/
/* EMFUNCTION                                                             */
/*****************************************************************************/

struct EMFunctionConfig {
    EMFunctionConfig()
        : select(SelectExpression::parse("*")),
          where(SqlExpression::parse("true"))
    {
    }
    
    PolyConfigT<Dataset> centroids;        ///< Dataset containing the centroids
    SelectExpression select;               ///< What to select from dataset
    std::shared_ptr<SqlExpression> where;  ///< Which centroids to take
};

DECLARE_STRUCTURE_DESCRIPTION(EMFunctionConfig);

struct EMFunction: public Function {
    EMFunction(MldbServer * owner,
                PolyConfig config,
                const std::function<bool (const Json::Value &)> & onProgress);
    
    virtual Any getStatus() const;
    
    virtual FunctionOutput apply(const FunctionApplier & applier,
                              const FunctionContext & context) const;
    
    /** Describe what the input and output is for this function. */
    virtual FunctionInfo getFunctionInfo() const;
    
    EMFunctionConfig functionConfig;
    std::vector<ColumnName> columnNames;
    int numDim;

     struct Cluster {
        CellValue clusterName;
        ML::distribution<float> centroid;
        boost::numeric::ublas::matrix<double> covarianceMatrix;
    };

    std::vector<Cluster> clusters;
};

} // namespace MLDB
} // namespace Datacratic