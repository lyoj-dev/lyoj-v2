# 评测机标准文件

## 评测状态枚举

```cpp
enum JudgeResult {
    AC = 0,
    WA = 1,
    TLE = 2,
    MLE = 3,
    RE = 4,
    NTD = 5, // 无测试点 No Test Data
    UE = 6,
    PC = 7,
    CE = 8,
    Waiting = 9,
    Compiling = 10,
    Running = 11,
    Skipped = 12,
    Submitted = 13
};
```

## Special Judge

格式与 `Lemon` 评测系统的相同。

对于使用 `testlib.h` 的用户，我们也提供了我们自制的 `testlib.h`，使用方法与原生的 `testlib.h` 相同。

SPJ 运行参数：`./spj <input> <output> <answer> <score> <info> <source> [[ extra... ]]`

对于不使用 `testlib.h` 的用户，您需要将得分写入到 `<score>` 文件中，并将评测信息写入到 `<info>` 文件中。

## 数据配置文件

应放置于 `{{ problem }}/{{ pid }}/config.json` 中。

### 主结构

|属性名|类型|说明|
|-|-|-|
|`input`|`string`|输入文件，`stdin` 表示标准输入|
|`output`|`string`|输出文件，`stdout` 表示标准输出|
|`spj`|`SpecialJudgeConfig`|SPJ 配置|
|`datas`|`DataConfig[]`|数据点配置|
|`subtasks`|`SubtaskConfig[]`|子任务配置|

### `SpecialJudgeConfig` 结构

|属性名|类型|说明|
|-|-|-|
|`type`|`int`|SPJ 类型，`0` 表示自定义 SPJ，否则表示 `judge.json` 中预先配置的 SPJ|
|`source`|`string`|SPJ 源文件名，当且仅当为自定义 SPJ 时填写|
|`compile_cmd`|`string`|SPJ 编译指令，当且仅当为自定义 SPJ 时填写|
|`exec_path`|`string`|已弃用。~~SPJ 生成路径，当且仅当为自定义 SPJ 时填写~~|
|`exec_name`|`string`|SPJ 可执行文件名，当且仅当为自定义 SPJ 时填写|
|`exec_param`|`string`|SPJ 额外运行参数|

### `DataConfig` 结构

|属性名|类型|说明|
|-|-|-|
|`input`|`string`|输入文件名|
|`output`|`string`|答案文件名|
|`score`|`int`|分数|
|`time`|`int`|时间限制|
|`memory`|`int`|空间限制|
|`subtask`|`int`|所属子任务编号，`0` 表示不属于任何一个子任务|

### `SubtaskConfig` 结构

|属性名|类型|说明|
|-|-|-|
|`id`|`int`|子任务编号|
|`title`|`string`|子任务名称|
|`type`|`int`|得分类型，`0` 表示求和，`1` 表示最大值，`2` 表示最小值，`3` 表示平均值|
|`depends`|`int[]`|依赖子任务编号|