# Stats Table Procedure

For a combination of feature columns (the `training_data` parameter) and outcomes, this procedure
will iteratively (over the rows) calculate different statistics (count, avg, std, min, max) on
the outcomes, for each values of the feature columns. 

The feature columns are assumed to be string-valued, and the outcome numeric-valued (type `INTEGER` or `NUMBER`). 

This procedure can be views as a ![](%%doclink statsTable.bagOfWords.train procedure) where the
labels are numbers instead of booleans.

You can optionally specity an output dataset that will contain the computed rolling statistics for each feature column / outcome
combination. Since we are doing it iteratively, it means that the statistics for row `n` will only use the `n-1` preceding rows.

The resulting statistical tables can be persisted using the `distTableFileUrl` parameter
and used later on to lookup counts using the ![](%%doclink distTable.getstats function).

## Configuration

![](%%config procedure distTable.train)

## Example

Let's assume a dataset made up of data from a real-time bidding online campaign. Each row
represents a bid request and each column represents a field in the bid request. We're interested
in tracking statistics for each possible values of each fieldoutcomes. The outcome we will be
interesed in here is the `purchase_value`.

|  rowName   |  host  |  region  | purchase_value |
|--|--|--|--|
| br_1     | patate.com  | on | 1 |
| br_2     | carotte.net | on | 2 |
| br_3     | patate.com  | on | 3 |
| br_4     | carotte.net | qc | 4 |

Assume this partial configuration:

```javascript
{
    "trainingData": "* EXCLUDING(purchase_value)",
    "outcomes": [
        ["price", "purchase_value"]
    ]
}
```

Here is an excerp of the statistics tables (as saved in `statsTableFileUrl`)

| outcome | column | value | stat | stat_value |
|--|--|--|--|--|
| price | host | patate.com | count | 2 |
| price | host | patate.com | avg | 2 |
| price | host | patate.com | std | 1.4142 |
| price | ... | ... | ... | ... |
| price | region | on | std | 1 |
| price | region | on | min | 1 |
| price | region | on | max | 3 |


The iteratively calculated dataset would look like this
The output dataset will contain:

| rowName | price.host.count | price.host.avg | price.host.std | ... | price.region.std | price.region.min | price.max |
|--|--|--|--|--|--|--|--|
| br_1 | 0 | `NULL` | `NULL` | ... | `NULL` | `NULL` | `NULL` |
| br_2 | 0 | `NULL` | `NULL` | ... | `NULL` | 1 | 1 |
| br_3 | 1 | 1 | `NULL` | ... | 1.4142 | 1 | 2 |
| br_4 | 1 | 2 | `NULL` | ... | `NULL` | `NULL` | `NULL` |

The idea behind that dataset is that if we sort those bid requests in the order they appeared,
this yields statistics about the statistics of the outcomes corresponding to the features of 
that bid request right before it was issued.

## See also
* The ![](%%doclink distTable.getStats function) does a lookup in stats tables for an input row.