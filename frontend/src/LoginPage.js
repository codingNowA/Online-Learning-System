// LoginPage.js
import React, { useState } from "react";
import axios from "axios";
import "./LoginPage.css";

export default function LoginPage({ onLogin }) {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState("");

  const handleLogin = async (e) => {
    e.preventDefault();
    if (!username || !password) {
      setError("请输入用户名和密码");
      return;
    }

    setLoading(true);
    setError("");

    try {
      const res = await axios.post("http://localhost:18080/user/login", {
        username,
        password,
      });
      onLogin(res.data.role, username);
    } catch (err) {
      setError(err.response?.data?.message || "登录失败");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="login-container">
      <form className="login-form" onSubmit={handleLogin}>
        <h2>课程管理系统</h2>
        {error && <div className="error-message">{error}</div>}
        <div className="form-group">
          <input
            placeholder="用户名"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            disabled={loading}
          />
        </div>
        <div className="form-group">
          <input
            placeholder="密码"
            type="password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            disabled={loading}
          />
        </div>
        <button type="submit" disabled={loading} className="login-button">
          {loading ? "登录中..." : "登录"}
        </button>
      </form>
    </div>
  );
}
