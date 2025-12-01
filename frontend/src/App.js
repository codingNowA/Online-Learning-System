// App.js (优化版)
import React, { useState } from "react";
import LoginPage from "./LoginPage";
import StudentDashboard from "./StudentDashboard";
import TeacherDashboard from "./TeacherDashboard";
import "./App.css";

function App() {
  const [userInfo, setUserInfo] = useState(null);

  const handleLogin = (role, username) => {
    setUserInfo({ role, username });
  };

  const handleLogout = () => {
    setUserInfo(null);
  };

  if (!userInfo) {
    return <LoginPage onLogin={handleLogin} />;
  }

  return (
    <div className="App">
      <header className="App-header">
        <h1>课程管理系统</h1>
        <div className="user-info">
          <span>欢迎, {userInfo.username}</span>
          <span>({userInfo.role === "student" ? "学生" : "教师"})</span>
          <button onClick={handleLogout} className="logout-button">
            退出登录
          </button>
        </div>
      </header>
      <main className="App-main">
        {userInfo.role === "student" && (
          <StudentDashboard username={userInfo.username} />
        )}
        {userInfo.role === "teacher" && (
          <TeacherDashboard username={userInfo.username} />
        )}
      </main>
    </div>
  );
}

export default App;
