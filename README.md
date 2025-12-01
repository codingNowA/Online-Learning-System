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
- 发布课件（支持外部资源链接或上传 PDF 到服务器）

### 学生端
- 注册并选择课程 
- 查看已选课程 
- 查看可选课程 
- 查看未完成作业 
- 提交作业 
- 查看自己的提交记录 
- 查看并下载课程课件（支持外链或通过后端下载已上传的 PDF）

---
## Route定义
请阅读 `RouteIGN.txt`（已在其中列出所有后端路由及示例请求，包括课件上传/下载相关路由）。

---

## 课件/资源说明
- 资源存储方式：支持两种方式
	- 外部链接：教师在发布课件时填写 `resource_url`（如 `https://.../slides.pdf`），前端直接展示为可点击链接。
	- 本地上传：教师可以上传 PDF（通过 `POST /course/<id>/material/upload`），后端把文件保存到项目目录下的 `uploads/materials/`，并把 `resource_url` 写为 `/materials/files/<filename>`，学生端会把它拼接为 `http://<host>:<port>/materials/files/<filename>` 下载。

- 安全提示：当前仅做了基本的文件名净化和类型判断（前端限制为 PDF），生产环境应添加认证、文件大小限制、MIME 校验与病毒扫描。

---
## 🗂 数据库结构
请阅读databaseIGN.txt

---
## 主要文件结构
.vscode/  ---vscode任务配置文件夹\
bin/   ---存放可执行文件和动态链接库\
build/  ---存放中间生成文件\
frontend/  ---React前端源码\
include/  ---head_only方式的头文件\
src/  ---main.cpp

---
## include下头文件功能
- AssignmentRoutes.h  ---实现作业有关功能
- CourseRoutes.h  ---实现课程有关功能
- UserRoutes.h  ---实现创建用户（前端暂未实现），用户登录
- DBHelper.h  ---用于连接数据库
- UserRoleChecker.h  ---查询用户role（学生还是教师）
- Utils.h  ---实现URL解码
- CorsMiddleware.h  ---CROS中间件，实现跨域访问\


