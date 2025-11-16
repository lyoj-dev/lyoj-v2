# APIs

## 目录

- [杂项](#杂项)
    - [评测状态](#评测状态)
    - [公共错误码](#公共错误码)
- [认证与回复](#认证与回复)
    - [认证](#认证)
    - [json 回复格式](#json-回复格式)
    - [其他说明](#其他说明)
- [服务器设置](#服务器设置)
    - [获取服务器支持语言列表](#获取服务器支持语言列表)
    - [获取服务器 Special Judge 列表](#获取服务器-special-judge-列表)
- [题目](#题目)
    - [列举题目](#列举题目)
    - [列举所有题目简要信息](#列举所有题目简要信息)
    - [列举所有题目标签](#列举所有题目标签)
    - [新建标签](#新建标签)
    - [获取题目详细信息](#获取题目详细信息)
    - [删除题目](#删除题目)
    - [提交题目](#提交题目)
    - [获取测试数据简要信息](#获取测试数据简要信息)
    - [获取题目配置文件信息](#获取题目配置文件信息)
    - [创建/修改题目](#创建修改题目)
    - [上传题目数据包](#上传题目数据包)
    - [下载题目数据包](#下载题目数据包)
    - [下载单个数据](#下载单个数据)
- [提交](#提交)
    - [列举提交](#列举提交)
    - [获取提交详细信息](#获取提交详细信息)
    - [提交重测](#提交重测)
- [比赛](#比赛)
    - [列举比赛](#列举比赛)
    - [获取比赛详细信息](#获取比赛详细信息)
    - [报名比赛](#报名比赛)
    - [取消报名比赛](#取消报名比赛)
    - [列举比赛题目](#列举比赛题目)
    - [列举所有比赛题目简要信息](#列举所有比赛题目简要信息)
    - [获取比赛题目详细信息](#获取比赛题目详细信息)
    - [提交比赛题目](#提交比赛题目)
    - [获取赛题测试数据简要信息](#获取赛题测试数据简要信息)
    - [列举比赛提交](#列举比赛提交)
    - [获取比赛提交详细信息](#获取比赛提交详细信息)
    - [列举所有已报名用户简要信息](#列举所有已报名用户简要信息)
    - [获取比赛排名](#获取比赛排名)
    - [创建/修改比赛](#创建修改比赛)

## 杂项

### 评测状态

|id|英文名|含义|
|-|-|-|
|0|Accepted|提交通过|
|1|Wrong Answer|答案错误|
|2|Time Limit Exceeded|运行超时|
|3|Memory Limit Exceeded|运行空间超限|
|4|Runtime Error|运行时错误|
|5|No Test Data|无测试数据|
|6|Unknown Error|未知错误|
|7|Partical Correct|部分正确|
|8|Compile Error|编译错误|
|9|Waiting...|等待评测|
|10|Compiling...|编译中|
|11|Running...|评测中|
|12|Skipped|跳过测试（一般是由于未能满足子任务依赖）|
|13|Submitted|已提交（仅在比赛中会出现）|

### 公共错误码

|代码|含义|
|-|-|
|200|没有问题~❤|
|400|服务器内部错误，请联系管理员|
|401|无权限访问此接口，请登录后重试|
|403|无权限访问此接口，请使用更高权限账号登录后重试|
|404|必要资源无法通过参数找到|
|405|请求方法出错|
|500|服务器内部出错（仅会在 CAS 登录时出错）|

## 认证与回复

### 认证

所有 api 均采用 Authorization Header 格式进行认证，格式为：`Authorization: SessionToken <token>`。

Token 需要通过登录 api 获取，有效时长 30 分钟。

如果 token 不存在或被验证无效，用户 id 将会返回为 0，表示未登录，部分接口可能将无法使用。

### json 回复格式

根对象：

|字段|类型|内容|备注|
|-|-|-|-|
|code|num|返回值||
|message|str|错误信息||
|loginAs|num|登录用户 id|`0` 为未登录<br>其他为当前登录用户 id|
|loginInfo|obj|登录用户信息||
|item/items|obj/array|回复正文，当且仅当 `code == 200` 时存在||

`loginInfo` 对象：

|字段|类型|内容|备注|
|-|-|-|-|
|title|str|用户名||
|groups|array|用户所属用户组||
|permission|num|用户权限||

`loginInfo` 中的 `groups` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|num|第 1 用户组 id||
|n|num|第 (n + 1) 用户组 id||
|......|num|......|......|

**后续文档仅展示 item 或 items 内容，如根对象有额外内容，将会单独列出。**

### 其他说明

所有 POST 接口正文参数均为 `application/json`，部分接口除外，将另外标出。后续文档将不再赘述。

例如下列是新建标签时需要发送的正文参数：

```json
{
    "title": "树链剖分",
    "type": 0
}
```

压缩后即为 `{"title":"树链剖分","type":0}`。

## 服务器设置

### 获取服务器支持语言列表

> https://api-v2.lyoj.littleyang.com.cn/configurations/languages

请求方式：GET

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 种语言||
|n|obj|第 (n + 1) 种语言||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|type|num|语言类型|`0` 为编译型语言<br>`1` 为解释型语言|
|name|str|语言名||
|mode|str|渲染语言类型||
|source_path|str|源文件路径||
|command|str|编译指令||
|exec_command|str|运行指令||

### 获取服务器 Special Judge 列表

> https://api-v2.lyoj.littleyang.com.cn/configurations/spjs

请求方式：GET

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 种 spj||
|n|obj|第 (n + 1) 种 spj||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|title|str|spj 标题||
|path|str|spj 路径||

## 题目

### 列举题目

> https://api-v2.lyoj.littleyang.com.cn/problems/list

请求方式：GET

**url 参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|page|num|页码|非必要|每页展示 10 题|
|title|str|题目标题|非必要||
|tags|str|标签 id 数组，使用 json 格式传递|非必要|数组内每个元素为标签 id，例如 `[1,2]`|
|minDiff|num|最小难度|非必要||
|maxDiff|num|最大难度|非必要||

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|pageCount|num|总页数||
|allowCreate|bool|是否允许创建题目||

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 道题目||
|n|obj|第 (n + 1) 道题目||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id||
|alias|str|题目别名||
|title|str|题目标题||
|difficulty|num|难度||
|tags|array|标签||
|accepted|num|通过提交数||
|total|num|总提交数||
|status|num|用户在此题的状态|`0` 为未提交<br>`1` 为已通过<br>`2` 为已提交未通过|
|allowEdit|bool|是否允许修改题目||
|allowDelete|bool|是否允许删除题目||

`items` 数组中的对象中的 `tags` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个标签||
|n|obj|第 (n + 1) 个标签||
|......|obj|......|......|

`items` 数组中的对象中的 `tags` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|标签 id||
|title|str|标签名||
|type|num|标签类型||

### 列举所有题目简要信息

> https://api-v2.lyoj.littleyang.com.cn/problems/listAll

请求方式：GET

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 道题目||
|n|obj|第 (n + 1) 道题目||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id||
|alias|str|题目别名||
|title|str|题目标题||

### 列举所有题目标签

> https://api-v2.lyoj.littleyang.com.cn/problems/listAllTags

请求方式：GET

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个标签||
|n|obj|第 (n + 1) 个标签||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|标签 id||
|title|str|标签名||
|type|num|标签类型||

### 新建标签

> https://api-v2.lyoj.littleyang.com.cn/problems/addTag

请求方式：POST

**正文参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|title|str|标签标题|必要||
|type|num|标签类型|必要||

**json 回复：**

无 `item` 对象或 `items` 数组，根对象上也无额外属性。

### 获取题目详细信息

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|config|obj|题目配置||

`item` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id||
|alias|str|题目别名||
|title|str|题目标题||
|difficulty|num|题目难度||
|tags|array|题目标签||
|bg|str|题目背景||
|descrip|str|题目描述||
|input|str|输入描述||
|output|str|输出描述||
|cases|array|测试样例||
|hint|str|数据范围与提示||
|langs|array|支持语言||
|groups|array|可见用户组||
|lastCode|str|用户上次提交代码|未登录时此栏恒为空|
|lastLang|num|用户上次使用语言 id|未登录时此栏恒为 `0`|

`item` 对象中的 `tags` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个标签||
|n|obj|第 (n + 1) 个标签||
|......|obj|......|......|

`item` 对象中的 `tags` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|标签 id||
|title|str|标签名||
|type|num|标签类型||

`item` 对象中的 `cases` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试样例||
|n|obj|第 (n + 1) 个测试样例||
|......|obj|......|......|

`item` 对象中的 `cases` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|input|str|输入样例||
|output|str|输出样例||

`item` 对象中的 `langs` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|num|第 1 个支持语言 id||
|n|num|第 (n + 1) 个支持语言 id||
|......|num|......|......|

`item` 对象中的 `groups` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|num|第 1 个可见用户组 id||
|n|num|第 (n + 1) 个可见用户组 id||
|......|num|......|......|

`config` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|input|str|输入文件|`stdin` 为标准输入|
|output|str|输出文件|`stdout` 为标准输出|
|minTime|num|最小时限|单位；`ms`|
|maxTime|num|最大时限|单位；`ms`|
|minMemory|num|最小空限|单位；`kb`|
|maxMemory|num|最大空限|单位；`kb`|

### 删除题目

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/delete

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**json 回复：**

无 `item` 对象或 `items` 数组，根对象上也无额外属性。

### 提交题目

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/submit

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**正文参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|lang|num|语言 id|必要||
|code|str|用户代码|必要||

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|提交 id||

无 `item` 对象或 `items` 数组。

### 获取测试数据简要信息

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/data

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|array|第 1 个子任务||
|n|array|第 (n + 1) 个子任务||
|......|array|......|......|

`items` 数组中的数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试数据||
|n|obj|第 (n + 1) 个测试数据||
|......|v|......|......|

`items` 数组中的数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|input|str|部分输入数据||
|inputIgnored|num|省略掉的输入数据长度||
|output|str|部分答案数据||
|outputIgnored|num|省略掉的答案数据长度||
|inputName|str|输入文件名||
|outputName|str|答案文件名||

### 获取题目配置文件信息

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/config

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**json 回复：**

`item` 对象：

|字段|类型|内容|备注|
|-|-|-|-|
|input|str|输入文件|`stdin` 为标准输入|
|output|str|输出文件|`stdout` 为标准输出|
|spj|obj|spj 信息||
|datas|array|所有测试点信息||
|subtasks|array|子任务信息||

`item` 对象中的 `datas` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试点信息||
|n|obj|第 (n + 1) 个测试点信息||
|......|obj|......|......|

`item` 对象中的 `datas` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|input|str|输入文件名||
|output|str|答案文件名||
|score|num|测试点分数||
|time|num|时间限制|以 `ms` 为单位|
|memory|num|空间限制|以 `kb` 为单位|
|subtask|num|测试点所属子任务编号|`0` 为不属于任意子任务|

`item` 对象中的 `subtasks` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个子任务信息||
|n|obj|第 (n + 1) 个子任务信息||
|......|obj|......|......|

`item` 对象中的 `subtasks` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|子任务编号|该值不能为 `0`|
|title|str|子任务名称||
|type|num|得分类型|`0` 表示求和<br>`1` 表示最大值<br>`2` 表示最小值<br>`3` 表示平均值|
|depends|array|依赖子任务编号||

`item` 对象中的 `subtasks` 数组中的对象中的 `depends` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|num|第 1 个依赖子任务编号||
|n|num|第 (n + 1) 个依赖子任务编号||
|......|num|......|......|

`item` 对象中的 `spj` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|type|int|spj 类型|`0` 表示自定义 spj<br>否则表示服务器中预先配置的 spj|
|source|str|spj 源文件名|当且仅当为自定义 spj 时不为空|
|compile_cmd|str|spj 编译指令|当且仅当为自定义 spj 时不为空|
|exec_path|str|spj 生成路径|已弃用。~~当且仅当为自定义 spj 时不为空~~|
|exec_name|str|spj 可执行文件名|当且仅当为自定义 spj 时不为空|
|exec_param|str|spj 额外运行参数||

### 创建/修改题目

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/create

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要|`0` 为创建题目<br>否则为要修改的题目 id|

**正文参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|item|obj|题目信息|必要||
|config|obj|题目配置|必要||

`item` 对象：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|alias|str|题目别名|必要||
|title|str|题目标题|必要||
|bg|str|题目背景|必要||
|descrip|str|题目描述|必要||
|input|str|输入描述|必要||
|output|str|输出描述|必要||
|cases|array|测试样例|必要||
|hint|str|数据范围与提示|必要||
|tags|array|题目标签 id 数组|必要||
|langs|array|支持语言|必要||
|groups|array|可见用户组|必要||

`item` 对象中的 `tags` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|num|第 1 个标签 id|非必要||
|n|num|第 (n + 1) 个标签 id|非必要||
|......|num|......|......|

`item` 对象中的 `cases` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|obj|第 1 个测试样例|非必要||
|n|obj|第 (n + 1) 个测试样例|非必要||
|......|obj|......|......|

`item` 对象中的 `cases` 数组中的对象：

|字段|类型|内容|必要性|备注|
|-|-|-|-|-|
|input|str|输入样例|必要||
|output|str|输出样例|必要||

`item` 对象中的 `langs` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|num|第 1 个支持语言 id|非必要||
|n|num|第 (n + 1) 个支持语言 id|非必要||
|......|num|......|......|

`item` 对象中的 `groups` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|num|第 1 个可见用户组 id|非必要||
|n|num|第 (n + 1) 个可见用户组 id|非必要||
|......|num|......|......|

`config` 对象：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|input|str|输入文件|必要|`stdin` 为标准输入|
|output|str|输出文件|必要|`stdout` 为标准输出|
|spj|obj|spj 信息|必要||
|datas|array|所有测试点信息|必要||
|subtasks|array|子任务信息|必要||

`config` 对象中的 `datas` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|obj|第 1 个测试点信息|非必要||
|n|obj|第 (n + 1) 个测试点信息|非必要||
|......|obj|......|......|

`config` 对象中的 `datas` 数组中的对象：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|input|str|输入文件名|必要||
|output|str|答案文件名|必要||
|score|num|测试点分数|必要||
|time|num|时间限制|必要|以 `ms` 为单位|
|memory|num|空间限制|必要|以 `kb` 为单位|
|subtask|num|测试点所属子任务编号|必要|`0` 为不属于任意子任务|

`config` 对象中的 `subtasks` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|obj|第 1 个子任务信息|非必要||
|n|obj|第 (n + 1) 个子任务信息|非必要||
|......|obj|......|......|

`config` 对象中的 `subtasks` 数组中的对象：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|子任务编号|必要|该值不能为 `0`|
|title|str|子任务名称|必要||
|type|num|得分类型|必要|`0` 表示求和<br>`1` 表示最大值<br>`2` 表示最小值<br>`3` 表示平均值|
|depends|array|依赖子任务编号|必要||

`config` 对象中的 `subtasks` 数组中的对象中的 `depends` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|num|第 1 个依赖子任务编号|非必要||
|n|num|第 (n + 1) 个依赖子任务编号|非必要||
|......|num|......|......|

`config` 对象中的 `spj` 对象：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|type|int|spj 类型|必要|`0` 表示自定义 spj<br>否则表示服务器中预先配置的 spj|
|source|str|spj 源文件名|必要|当且仅当为自定义 spj 时不为空|
|compile_cmd|str|spj 编译指令|必要|当且仅当为自定义 spj 时不为空|
|exec_path|str|spj 生成路径|必要|已弃用。~~当且仅当为自定义 spj 时不为空~~|
|exec_name|str|spj 可执行文件名|必要|当且仅当为自定义 spj 时不为空|
|exec_param|str|spj 额外运行参数|必要||

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id|创建题目时返回新题目 id<br>修改题目时返回原题目 id|

### 上传题目数据包

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/data/upload

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**正文参数（plain/text）：**

|类型|内容|必要性|备注|
|-|-|-|-|
|str|数据包 zip 文件 base64 编码|必要||

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|datas|array|识别到的输入/答案文件对||

`data` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 对数据||
|n|obj|第 (n + 1) 对数据||
|......|obj|......|......|

`data` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|input|str|输入文件名||
|output|str|答案文件名||

### 下载题目数据包

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/data/download

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||

**回复正文（application/zip）：**

|类型|内容|备注|
|-|-|-|
|str|数据包 zip 文件|该文件未经过 base64 加密，直接为原始二进制文件|

### 下载单个数据

> https://api-v2.lyoj.littleyang.com.cn/problems/{id}/data/{name}

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|题目 id|必要||
|name|str|数据文件名|必要||

**回复正文（plain/text）：**

|类型|内容|备注|
|-|-|-|
|str|数据文件|该文件未经过 base64 加密，直接为原始数据文件|

## 提交

### 列举提交

> https://api-v2.lyoj.littleyang.com.cn/submissions/list

请求方式：GET

**url 参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|page|num|页码|非必要|每页展示 10 项提交|
|problems|str|题目 id 数组，使用 json 格式传递|非必要|数组内每个元素为题目 id，例如 `[1,2]`|
|users|str|用户 id 数组，使用 json 格式传递|非必要|数组内每个元素为用户 id，例如 `[1,2]`|
|languages|str|语言 id 数组，使用 json 格式传递|非必要|数组内每个元素为语言 id，例如 `[1,2]`|
|status|str|评测状态 id 数组，使用 json 格式传递|非必要|数组内每个元素为评测状态 id，例如 `[1,2]`。评测状态 id 详见[评测状态](评测状态)|

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 项提交||
|n|obj|第 (n + 1) 项提交||
|......|obj|......|......|

`items` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|提交 id||
|user|str|提交用户名||
|uid|num|提交用户 id||
|problem|str|题目名||
|pid|num|题目 id||
|statusType|num|评测状态 id|详见[评测状态](评测状态)|
|status|str|评测状态信息||
|judged|bool|是否评测完成||
|score|num|得分||

### 获取提交详细信息

> https://api-v2.lyoj.littleyang.com.cn/submissions/{id}

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|提交 id|必要||

**json 回复：**

`item` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|提交 id||
|pid|num|题目 id||
|uid|num|提交用户 id||
|code|str|用户提交代码|若无权限即则为空|
|info|str|编译信息|若无权限即则为空|
|date|num|提交评测时间戳|单位 `s`|
|problem|str|题目名||
|user|str|提交用户名||
|judged|bool|是否评测完成||
|rejudge|bool|是否允许提交重测||
|statusType|num|评测状态 id|详见[评测状态](评测状态)|
|status|str|评测状态信息||
|score|num|得分||
|time|num|评测总用时|单位 `s`|
|memory|num|最大运行内存|单位 `kb`|
|langCode|num|语言 id||
|langMode|str|代码渲染模式||
|langName|str|语言名||
|subtasks|array|子任务信息||

`item` 对象中的 `subtasks` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个子任务||
|n|obj|第 (n + 1) 个子任务||
|......|obj|......|......|

`item` 对象中的 `subtasks` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|子任务 id|`0` 为不属于任意子任务的数据|
|score|num|子任务得分||
|status|num|子任务评测状态 id|详见[评测状态](评测状态)|
|title|str|子任务标题||
|datas|array|子任务测试点信息||

`item` 对象中的 `subtasks` 数组中的对象中的 `datas` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试点||
|n|obj|第 (n + 1) 个测试点||
|......|obj|......|......|

`item` 对象中的 `subtasks` 数组中的对象中的 `datas` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|time|num|运行时间|单位 `s`|
|memory|num|最大运行内存|单位 `kb`|
|score|num|测试点得分||
|status|num|子任务评测状态 id|详见[评测状态](评测状态)|
|info|str|spj 输出信息||
|output|str|部分用户输出||
|outputIgnored|num|省略掉的用户输出长度||

### 提交重测

> https://api-v2.lyoj.littleyang.com.cn/submissions/{id}/rejudge

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|提交 id|必要||

**json 回复：**

无 `item` 对象或 `items` 数组，根对象上也无额外属性。

## 比赛

### 列举比赛

> https://api-v2.lyoj.littleyang.com.cn/contests/list

请求方式：GET

**url 参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|page|num|页码|非必要|每页展示 10 场比赛|

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 场比赛||
|n|obj|第 (n + 1) 场比赛||
|......|obj|......|......|

`items` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|比赛 id||
|title|str|比赛标题||
|starttime|num|比赛开始时间戳|单位 `s`|
|duration|num|比赛时长|单位 `s`|
|type|num|比赛类型|`0` 为 OI 赛制<br>`1` 为 IOI 赛制<br>`2` 为 ACM 赛制|
|rated|bool|是否计入评级||
|tags|array|比赛标签||
|problems|num|比赛题目数量||

`items` 数组中的对象中的 `tags` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|obj|第 1 个标签|非必要||
|n|obj|第 (n + 1) 个标签|非必要||
|......|obj|......|......|

`items` 数组中的对象中的 `tags` 数组中的对象：

|字段|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|标签 id|必要||
|title|str|标签名|必要||
|type|num|标签类型|必要||

### 获取比赛详细信息

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

`items` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|比赛 id||
|title|str|比赛标题||
|starttime|num|比赛开始时间戳|单位 `s`|
|duration|num|比赛时长|单位 `s`|
|type|num|比赛类型|`0` 为 OI 赛制<br>`1` 为 IOI 赛制<br>`2` 为 ACM 赛制|
|rated|bool|是否计入评级||
|info|str|比赛公告||
|tags|array|比赛标签||
|signups|num|报名人数||
|problemNumber|num|比赛题目数量||
|allowEdit|bool|是否允许修改比赛||
|allowDelete|bool|是否允许删除比赛||
|hasPassword|bool|比赛是否设置了密码||

`item` 对象中的 `tags` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|obj|第 1 个标签|非必要||
|n|obj|第 (n + 1) 个标签|非必要||
|......|obj|......|......|

`item` 对象中的 `tags` 数组中的对象：

|字段|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|标签 id|必要||
|title|str|标签名|必要||
|type|num|标签类型|必要||

### 报名比赛

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/register

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

无 `item` 对象或 `items` 数组，根对象上也无额外属性。

### 取消报名比赛

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/unregister

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

无 `item` 对象或 `items` 数组，根对象上也无额外属性。

### 列举比赛题目

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/problems/list

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 道题目||
|n|obj|第 (n + 1) 道题目||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id|此 id 为该题目在比赛中的 id|
|realId|num|真实题目 id||
|alias|str|题目别名||
|title|str|题目标题||
|difficulty|num|难度||
|tags|array|标签||
|accepted|num|通过提交数||
|total|num|总提交数||
|status|num|用户在此题的状态|`0` 为未提交<br>`1` 为已通过<br>`2` 为已提交未通过|
|allowEdit|bool|是否允许修改题目||
|allowDelete|bool|是否允许删除题目||

`items` 数组中的对象中的 `tags` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个标签||
|n|obj|第 (n + 1) 个标签||
|......|obj|......|......|

`items` 数组中的对象中的 `tags` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|标签 id||
|title|str|标签名||
|type|num|标签类型||

### 列举所有比赛题目简要信息

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/problems/listAll

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 道题目||
|n|obj|第 (n + 1) 道题目||
|......|obj|......|......|

`items` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id||
|alias|str|题目别名||
|title|str|题目标题||

### 获取比赛题目详细信息

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/problems/{pid}

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||
|pid|num|题目 id|必要|非真实题目 id，而是在该比赛中的题目 id|

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|config|obj|题目配置||

`item` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|题目 id||
|alias|str|题目别名|非真实题目别名，恒为 `#{pid}`|
|title|str|题目标题||
|difficulty|num|题目难度||
|tags|array|题目标签||
|bg|str|题目背景||
|descrip|str|题目描述||
|input|str|输入描述||
|output|str|输出描述||
|cases|array|测试样例||
|hint|str|数据范围与提示||
|langs|array|支持语言||
|groups|array|可见用户组||
|lastCode|str|用户上次提交代码|未登录时此栏恒为空|
|lastLang|num|用户上次使用语言 id|未登录时此栏恒为 `0`|

`item` 对象中的 `tags` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个标签||
|n|obj|第 (n + 1) 个标签||
|......|obj|......|......|

`item` 对象中的 `tags` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|id|num|标签 id||
|title|str|标签名||
|type|num|标签类型||

`item` 对象中的 `cases` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试样例||
|n|obj|第 (n + 1) 个测试样例||
|......|obj|......|......|

`item` 对象中的 `cases` 数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|input|str|输入样例||
|output|str|输出样例||

`item` 对象中的 `langs` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|num|第 1 个支持语言 id||
|n|num|第 (n + 1) 个支持语言 id||
|......|num|......|......|

`item` 对象中的 `groups` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|num|第 1 个可见用户组 id||
|n|num|第 (n + 1) 个可见用户组 id||
|......|num|......|......|

`config` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|input|str|输入文件|`stdin` 为标准输入|
|output|str|输出文件|`stdout` 为标准输出|
|minTime|num|最小时限|单位；`ms`|
|maxTime|num|最大时限|单位；`ms`|
|minMemory|num|最小空限|单位；`kb`|
|maxMemory|num|最大空限|单位；`kb`|

### 提交比赛题目

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/problems/{pid}/submit

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||
|pid|num|题目 id|必要|非真实题目 id，而是在该比赛中的题目 id|

**正文参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|lang|num|语言 id|必要||
|code|str|用户代码|必要||

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|提交 id||

无 `item` 对象或 `items` 数组。

### 获取赛题测试数据简要信息

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/problems/{pid}/data

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||
|pid|num|题目 id|必要|非真实题目 id，而是在该比赛中的题目 id|

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|array|第 1 个子任务||
|n|array|第 (n + 1) 个子任务||
|......|array|......|......|

`items` 数组中的数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试数据||
|n|obj|第 (n + 1) 个测试数据||
|......|v|......|......|

`items` 数组中的数组中的对象：

|字段|类型|内容|备注|
|-|-|-|-|
|input|str|部分输入数据||
|inputIgnored|num|省略掉的输入数据长度||
|output|str|部分答案数据||
|outputIgnored|num|省略掉的答案数据长度||
|inputName|str|输入文件名||
|outputName|str|答案文件名||

### 列举比赛提交

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/submissions/list

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**url 参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|page|num|页码|非必要|每页展示 10 项提交|
|problems|str|题目 id 数组，使用 json 格式传递|非必要|数组内每个元素为在该比赛中的题目 id，例如 `[1,2]`|
|users|str|用户 id 数组，使用 json 格式传递|非必要|数组内每个元素为用户 id，例如 `[1,2]`|
|languages|str|语言 id 数组，使用 json 格式传递|非必要|数组内每个元素为语言 id，例如 `[1,2]`|
|status|str|评测状态 id 数组，使用 json 格式传递|非必要|数组内每个元素为评测状态 id，例如 `[1,2]`。评测状态 id 详见[评测状态](评测状态)|

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 项提交||
|n|obj|第 (n + 1) 项提交||
|......|obj|......|......|

`items` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|提交 id||
|user|str|提交用户名||
|uid|num|提交用户 id||
|problem|str|题目名||
|pid|num|题目 id|非真实题目 id，而是在该比赛中的题目 id|
|statusType|num|评测状态 id|详见[评测状态](评测状态)|
|status|str|评测状态信息||
|judged|bool|是否评测完成||
|score|num|得分||

### 获取比赛提交详细信息

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/submissions/{sid}

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||
|sid|num|提交 id|必要||

**json 回复：**

`item` 对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|提交 id||
|pid|num|题目 id|非真实题目 id，而是在该比赛中的题目 id|
|uid|num|提交用户 id||
|code|str|用户提交代码|若无权限即则为空|
|info|str|编译信息|若无权限即则为空|
|date|num|提交评测时间戳|单位 `s`|
|problem|str|题目名||
|user|str|提交用户名||
|judged|bool|是否评测完成||
|rejudge|bool|是否允许提交重测||
|statusType|num|评测状态 id|详见[评测状态](评测状态)|
|status|str|评测状态信息||
|score|num|得分||
|time|num|评测总用时|单位 `s`|
|memory|num|最大运行内存|单位 `kb`|
|langCode|num|语言 id||
|langMode|str|代码渲染模式||
|langName|str|语言名||
|subtasks|array|子任务信息||

`item` 对象中的 `subtasks` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个子任务||
|n|obj|第 (n + 1) 个子任务||
|......|obj|......|......|

`item` 对象中的 `subtasks` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|子任务 id|`0` 为不属于任意子任务的数据|
|score|num|子任务得分||
|status|num|子任务评测状态 id|详见[评测状态](评测状态)|
|title|str|子任务标题||
|datas|array|子任务测试点信息||

`item` 对象中的 `subtasks` 数组中的对象中的 `datas` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 个测试点||
|n|obj|第 (n + 1) 个测试点||
|......|obj|......|......|

`item` 对象中的 `subtasks` 数组中的对象中的 `datas` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|time|num|运行时间|单位 `s`|
|memory|num|最大运行内存|单位 `kb`|
|score|num|测试点得分||
|status|num|子任务评测状态 id|详见[评测状态](评测状态)|
|info|str|spj 输出信息||
|output|str|部分用户输出||
|outputIgnored|num|省略掉的用户输出长度||

### 列举所有已报名用户简要信息

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/users/listAll

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 位用户||
|n|obj|第 (n + 1) 位用户||
|......|obj|......|......|

`items` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|用户 id||
|title|str|用户名||

### 获取比赛排名

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/ranking

请求方式：GET

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要||

**json 回复：**

`items` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 位用户||
|n|obj|第 (n + 1) 位用户||
|......|obj|......|......|

`items` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|用户排名||
|uid|num|用户 id||
|user|str|用户名||
|score|num|用户得分||
|time|num|程序最优运行时间总和|单位 `ms`|
|time2|num|首次通过时间总和|单位 `s`|
|penalty|num|罚时数|此处仅为首次正确提交前错误提交次数总和|
|info|array|赛题提交信息||

`items` 数组中的对象中的 `info` 数组：

|项|类型|内容|备注|
|-|-|-|-|
|0|obj|第 1 道赛题提交信息||
|n|obj|第 (n + 1) 道赛题提交信息||
|......|obj|......|......|

`items` 数组中的对象中的 `info` 数组中的对象：

|项|类型|内容|备注|
|-|-|-|-|
|pid|num|题目 id|非真实题目 id，而是在该比赛中的题目 id|
|score|num|用户得分||
|sid|num|相关提交 id||
|time|num|程序最优运行时间|单位 `ms`|
|time2|num|首次通过时间|单位 `s`|
|penalty|num|罚时数|此处仅为首次正确提交前错误提交次数|

### 创建/修改比赛

> https://api-v2.lyoj.littleyang.com.cn/contests/{id}/create

请求方式：POST

**路径参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|id|num|比赛 id|必要|`0` 为创建比赛<br>否则为要修改的比赛 id|

**正文参数：**

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|title|str|比赛标题|必要||
|starttime|num|比赛开始时间戳|必要|单位 `s`|
|duration|num|比赛时长|必要|单位 `s`|
|type|num|比赛类型|必要|`0` 为 OI 赛制<br>`1` 为 IOI 赛制<br>`2` 为 ACM 赛制|
|rated|bool|是否计入评级|必要||
|tags|array|比赛标签 id 数组|必要||
|problems|array|比赛题目 id 数组|必要||
|password|str|md5 加密后的比赛密码|必要|若无则为空|
|info|str|比赛公告|必要||

`tags` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|num|第 1 个标签 id|非必要||
|n|num|第 (n + 1) 个标签 id|非必要||
|......|num|......|......|

`problems` 数组：

|项|类型|内容|必要性|备注|
|-|-|-|-|-|
|0|num|第 1 道题目 id|非必要|填写真实题目 id|
|n|num|第 (n + 1) 道题目 id|非必要|填写真实题目 id|
|......|num|......|......|

**json 回复：**

根对象：

|项|类型|内容|备注|
|-|-|-|-|
|id|num|比赛 id|创建比赛时返回新比赛 id<br>修改比赛时返回原比赛 id|