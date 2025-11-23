import React from "react";
import { BrowserRouter as Router, Route, Routes, Link } from "react-router-dom";
import CoursePage from "./pages/CoursePage";
import AssignmentPage from "./pages/AssignmentPage";
import UserPage from "./pages/UserPage";

function App() {
  return (
    <Router>
      <nav>
        <Link to="/users">用户管理</Link> | 
        <Link to="/courses">课程</Link> | 
        <Link to="/assignments">作业</Link>
      </nav>
      <Routes>
        <Route path="/users" element={<UserPage />} />
        <Route path="/courses" element={<CoursePage />} />
        <Route path="/assignments" element={<AssignmentPage />} />
      </Routes>
    </Router>
  );
}

export default App;


/*
import React from "react";
import axios from "axios";

function App() {
  const testBackend = async () => {
    try {
      const res = await axios.post("http://localhost:18080/hello");
      alert("后端返回: " + res.data);
    } catch (err) {
      alert("请求失败: " + err.message);
    }
  };

  return (
    <div style={{ padding: "20px" }}>
      <h1>前后端连通测试</h1>
      <button onClick={testBackend}>测试调用后端</button>
    </div>
  );
}

export default App;
*/