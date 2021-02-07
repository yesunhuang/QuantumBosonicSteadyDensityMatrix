# Git 协作

为了更好的管理 Git 分支，我们会有 master 以及 dev 分支。

master 分支用来发布主要的版本。

dev 分支用于同步主要的开发进展。

个人开发时，需要创建新分支，分支名的格式如 `[开发者]/[分支功能]`，例如 `ayajilin/algorithm` 表示为开发者 ayajilin 开发算法的分支。个人开发分支结束后，需要通过 **rebase** 先 rebase 到 dev 分支，再进行合并提交。

共勉。
