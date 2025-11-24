# 在线学习平台 (Online Learning System)

一个基于 **C++ Crow 后端 + React 前端** 的在线学习平台，支持教师和学生的完整教学流程。

---

## ✨ 功能特性

### 教师端
- 创建课程 
- 布置作业 
- 查看已布置的课程与作业
- 查看学生提交情况 
- 给学生提交打分与评论 

### 学生端
- 注册并选择课程 
- 查看已选课程 
- 查看可选课程 
- 查看未完成作业 
- 提交作业 
- 查看自己的提交记录 

---
## Route定义
请阅读RouteIGN.txt

---
## 🗂 数据库结构
请阅读databaseIGN.txt

---
## 主要文件结构
.vscode/  ---vscode任务配置文件夹\
bin/      ---存放可执行文件和动态链接库\
build/    ---存放中间生成文件\
frontend/ ---React前端源码\
include/  ---head_only方式的头文件\
src/      ---main.cpp

---
## include下头文件功能
- AssignmentRoutes.h  ---实现作业有关功能
- CourseRoutes.h      ---实现课程有关功能
- UserRoutes.h        ---实现创建用户（前端暂未实现），用户登录
- DBHelper.h          ---用于连接数据库
- UserRoleChecker.h   ---查询用户role（学生还是教师）
- Utils.h             ---实现URL解码
- CorsMiddleware.h    ---CROS中间件，实现跨域访问


