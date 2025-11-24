// CoursePage.js
import React, { useState } from "react";
import axios from "axios";
import "./FormPage.css";

export default function CoursePage() {
  const [createForm, setCreateForm] = useState({
    courseName: "",
    teacher: ""
  });
  const [enrollForm, setEnrollForm] = useState({
    courseId: "",
    student: ""
  });
  const [loading, setLoading] = useState({ create: false, enroll: false });
  const [message, setMessage] = useState("");

  const createCourse = async (e) => {
    e.preventDefault();
    setLoading(prev => ({ ...prev, create: true }));
    try {
      const res = await axios.post("http://localhost:18080/course/create", {
        name: createForm.courseName,
        teacher: createForm.teacher,
      });
      setMessage(`课程创建成功: ${res.data}`);
      setCreateForm({ courseName: "", teacher: "" });
    } catch (err) {
      setMessage("创建失败: " + (err.response?.data?.message || err.message));
    } finally {
      setLoading(prev => ({ ...prev, create: false }));
    }
  };

  const enrollCourse = async (e) => {
    e.preventDefault();
    setLoading(prev => ({ ...prev, enroll: true }));
    try {
      const res = await axios.post(`http://localhost:18080/course/${enrollForm.courseId}/enroll`, {
        student: enrollForm.student,
      });
      setMessage(`选课成功: ${res.data}`);
      setEnrollForm({ courseId: "", student: "" });
    } catch (err) {
      setMessage("选课失败: " + (err.response?.data?.message || err.message));
    } finally {
      setLoading(prev => ({ ...prev, enroll: false }));
    }
  };

  return (
    <div className="page-container">
      <div className="form-section">
        <h2>课程管理</h2>
        {message && <div className="message">{message}</div>}
        
        <div className="form-card">
          <h3>教师创建课程</h3>
          <form onSubmit={createCourse}>
            <div className="form-group">
              <input
                placeholder="课程名"
                value={createForm.courseName}
                onChange={(e) => setCreateForm(prev => ({ ...prev, courseName: e.target.value }))}
                disabled={loading.create}
              />
            </div>
            <div className="form-group">
              <input
                placeholder="教师用户名"
                value={createForm.teacher}
                onChange={(e) => setCreateForm(prev => ({ ...prev, teacher: e.target.value }))}
                disabled={loading.create}
              />
            </div>
            <button type="submit" disabled={loading.create} className="submit-button">
              {loading.create ? "创建中..." : "创建课程"}
            </button>
          </form>
        </div>

        <div className="form-card">
          <h3>学生选课</h3>
          <form onSubmit={enrollCourse}>
            <div className="form-group">
              <input
                placeholder="课程ID"
                value={enrollForm.courseId}
                onChange={(e) => setEnrollForm(prev => ({ ...prev, courseId: e.target.value }))}
                disabled={loading.enroll}
              />
            </div>
            <div className="form-group">
              <input
                placeholder="学生用户名"
                value={enrollForm.student}
                onChange={(e) => setEnrollForm(prev => ({ ...prev, student: e.target.value }))}
                disabled={loading.enroll}
              />
            </div>
            <button type="submit" disabled={loading.enroll} className="submit-button">
              {loading.enroll ? "选课中..." : "选课"}
            </button>
          </form>
        </div>
      </div>
    </div>
  );
}
