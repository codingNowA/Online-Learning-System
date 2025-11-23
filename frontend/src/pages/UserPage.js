import React, { useState } from "react";
import axios from "axios";

export default function UserPage() {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [role, setRole] = useState("student");

  const createUser = async () => {
    try {
      const res = await axios.post("http://localhost:18080/user/create", {
        username,
        password,
        role,
      });
      alert(res.data);
    } catch (err) {
      alert("Error: " + err.message);
    }
  };

  return (
    <div>
      <h2>创建用户</h2>
      <input placeholder="用户名" onChange={(e) => setUsername(e.target.value)} />
      <input placeholder="密码" type="password" onChange={(e) => setPassword(e.target.value)} />
      <select onChange={(e) => setRole(e.target.value)}>
        <option value="student">学生</option>
        <option value="teacher">教师</option>
      </select>
      <button onClick={createUser}>创建</button>
    </div>
  );
}
