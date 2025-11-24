// LoginPage.js
import React, { useState } from "react";
import "./LoginPage.css";

function LoginPage({ onLogin }) {
  const [formData, setFormData] = useState({
    username: "",
    password: ""
  });
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState("");

  const handleChange = (field, value) => {
    setFormData(prev => ({ ...prev, [field]: value }));
    setError(""); // 输入时清空错误信息
  };

  const handleLogin = async (e) => {
    e?.preventDefault(); // 支持表单提交和按钮点击
    if (!formData.username || !formData.password) {
      setError("请输入用户名和密码");
      return;
    }

    setLoading(true);
    setError("");

    try {
      const res = await fetch("http://localhost:18080/login", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ 
          username: formData.username, 
          password: formData.password 
        })
      });
      
      if (!res.ok) {
        const errorData = await res.json().catch(() => ({}));
        throw new Error(errorData.message || `登录失败: ${res.status}`);
      }
      
      const data = await res.json();
      
      // 根据后端实际响应结构调整
      const userRole = data.role || data.data?.role;
      if (!userRole) {
        throw new Error("无效的响应格式");
      }
      
      onLogin(userRole, formData.username);
    } catch (err) {
      setError(err.message || "登录失败，请重试");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="login-container">
      <form className="login-form" onSubmit={handleLogin}>
        <h2>课程管理系统</h2>
        <p className="login-subtitle">请登录您的账户</p>
        
        {error && <div className="error-message">{error}</div>}
        
        <div className="form-group">
          <input
            type="text"
            placeholder="用户名"
            value={formData.username}
            onChange={e => handleChange("username", e.target.value)}
            disabled={loading}
            className="login-input"
          />
        </div>
        
        <div className="form-group">
          <input
            type="password"
            placeholder="密码"
            value={formData.password}
            onChange={e => handleChange("password", e.target.value)}
            disabled={loading}
            className="login-input"
          />
        </div>
        
        <button 
          type="submit" 
          disabled={loading} 
          className={`login-button ${loading ? 'loading' : ''}`}
        >
          {loading ? (
            <>
              <span className="loading-spinner"></span>
              登录中...
            </>
          ) : (
            "登录"
          )}
        </button>
        
        <div className="login-footer">
          <p>演示账号</p>
          <p>学生: student / 123456</p>
          <p>教师: teacher / 123456</p>
        </div>
      </form>
    </div>
  );
}

export default LoginPage;
