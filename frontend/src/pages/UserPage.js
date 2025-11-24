// UserPage.js
import React, { useState } from "react";
import axios from "axios";
import "./FormPage.css";

export default function UserPage() {
  const [formData, setFormData] = useState({
    username: "",
    password: "",
    role: "student"
  });
  const [loading, setLoading] = useState(false);
  const [message, setMessage] = useState("");

  const handleChange = (field, value) => {
    setFormData(prev => ({ ...prev, [field]: value }));
  };

  const createUser = async (e) => {
    e.preventDefault();
    if (!formData.username || !formData.password) {
      setMessage("请填写完整信息");
      return;
    }

    setLoading(true);
    try {
      const res = await axios.post("http://localhost:18080/user/create", formData);
      setMessage("用户创建成功");
      setFormData({ username: "", password: "", role: "student" });
    } catch (err) {
      setMessage("创建失败: " + (err.response?.data?.message || err.message));
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="page-container">
      <div className="form-card">
        <h2>创建用户</h2>
        {message && <div className="message">{message}</div>}
        <form onSubmit={createUser}>
          <div className="form-group">
            <input
              placeholder="用户名"
              value={formData.username}
              onChange={(e) => handleChange("username", e.target.value)}
              disabled={loading}
            />
          </div>
          <div className="form-group">
            <input
              placeholder="密码"
              type="password"
              value={formData.password}
              onChange={(e) => handleChange("password", e.target.value)}
              disabled={loading}
            />
          </div>
          <div className="form-group">
            <select
              value={formData.role}
              onChange={(e) => handleChange("role", e.target.value)}
              disabled={loading}
            >
              <option value="student">学生</option>
              <option value="teacher">教师</option>
            </select>
          </div>
          <button type="submit" disabled={loading} className="submit-button">
            {loading ? "创建中..." : "创建用户"}
          </button>
        </form>
      </div>
    </div>
  );
}
