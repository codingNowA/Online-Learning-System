// AssignmentPage.js
import React, { useState } from "react";
import axios from "axios";
import "./FormPage.css";

export default function AssignmentPage() {
  const [forms, setForms] = useState({
    create: { courseId: "", teacher: "", title: "", description: "", dueDate: "" },
    submit: { courseId: "", student: "", content: "" },
    grade: { submissionId: "", teacher: "", grade: "", comments: "" }
  });
  const [loading, setLoading] = useState({ create: false, submit: false, grade: false });
  const [message, setMessage] = useState("");

  const handleFormChange = (formType, field, value) => {
    setForms(prev => ({
      ...prev,
      [formType]: { ...prev[formType], [field]: value }
    }));
  };

  const createAssignment = async (e) => {
    e.preventDefault();
    setLoading(prev => ({ ...prev, create: true }));
    try {
      await axios.post("http://localhost:18080/assignment/create", {
        course_id: parseInt(forms.create.courseId),
        teacher: forms.create.teacher,
        title: forms.create.title,
        description: forms.create.description,
        due_date: forms.create.dueDate,
      });
      setMessage("作业创建成功");
      setForms(prev => ({ ...prev, create: { courseId: "", teacher: "", title: "", description: "", dueDate: "" } }));
    } catch (err) {
      setMessage("创建失败: " + (err.response?.data?.message || err.message));
    } finally {
      setLoading(prev => ({ ...prev, create: false }));
    }
  };

  const submitAssignment = async (e) => {
    e.preventDefault();
    setLoading(prev => ({ ...prev, submit: true }));
    try {
      await axios.post(`http://localhost:18080/assignment/${forms.submit.courseId}/submit`, {
        student: forms.submit.student,
        content: forms.submit.content,
      });
      setMessage("作业提交成功");
      setForms(prev => ({ ...prev, submit: { courseId: "", student: "", content: "" } }));
    } catch (err) {
      setMessage("提交失败: " + (err.response?.data?.message || err.message));
    } finally {
      setLoading(prev => ({ ...prev, submit: false }));
    }
  };

  const gradeAssignment = async (e) => {
    e.preventDefault();
    setLoading(prev => ({ ...prev, grade: true }));
    try {
      await axios.post(`http://localhost:18080/assignment/${forms.grade.submissionId}/grade`, {
        teacher: forms.grade.teacher,
        grade: forms.grade.grade,
        comments: forms.grade.comments,
      });
      setMessage("评分成功");
      setForms(prev => ({ ...prev, grade: { submissionId: "", teacher: "", grade: "", comments: "" } }));
    } catch (err) {
      setMessage("评分失败: " + (err.response?.data?.message || err.message));
    } finally {
      setLoading(prev => ({ ...prev, grade: false }));
    }
  };

  return (
    <div className="page-container">
      <h2>作业管理</h2>
      {message && <div className="message">{message}</div>}
      
      <div className="form-section">
        <div className="form-card">
          <h3>教师布置作业</h3>
          <form onSubmit={createAssignment}>
            {["courseId", "teacher", "title", "description", "dueDate"].map(field => (
              <div className="form-group" key={field}>
                <input
                  placeholder={field === "dueDate" ? "截止日期 (YYYY-MM-DD)" : 
                              field === "courseId" ? "课程ID" :
                              field === "teacher" ? "教师用户名" :
                              field === "title" ? "标题" : "描述"}
                  value={forms.create[field]}
                  onChange={(e) => handleFormChange("create", field, e.target.value)}
                  disabled={loading.create}
                />
              </div>
            ))}
            <button type="submit" disabled={loading.create} className="submit-button">
              {loading.create ? "布置中..." : "布置作业"}
            </button>
          </form>
        </div>

        <div className="form-card">
          <h3>学生提交作业</h3>
          <form onSubmit={submitAssignment}>
            {["courseId", "student"].map(field => (
              <div className="form-group" key={field}>
                <input
                  placeholder={field === "courseId" ? "课程ID" : "学生用户名"}
                  value={forms.submit[field]}
                  onChange={(e) => handleFormChange("submit", field, e.target.value)}
                  disabled={loading.submit}
                />
              </div>
            ))}
            <div className="form-group">
              <textarea
                placeholder="作业内容"
                value={forms.submit.content}
                onChange={(e) => handleFormChange("submit", "content", e.target.value)}
                disabled={loading.submit}
                rows="4"
              />
            </div>
            <button type="submit" disabled={loading.submit} className="submit-button">
              {loading.submit ? "提交中..." : "提交作业"}
            </button>
          </form>
        </div>

        <div className="form-card">
          <h3>教师评分</h3>
          <form onSubmit={gradeAssignment}>
            {["submissionId", "teacher", "grade", "comments"].map(field => (
              <div className="form-group" key={field}>
                <input
                  placeholder={field === "submissionId" ? "提交ID" :
                              field === "teacher" ? "教师用户名" :
                              field === "grade" ? "成绩" : "评语"}
                  value={forms.grade[field]}
                  onChange={(e) => handleFormChange("grade", field, e.target.value)}
                  disabled={loading.grade}
                />
              </div>
            ))}
            <button type="submit" disabled={loading.grade} className="submit-button">
              {loading.grade ? "评分中..." : "评分"}
            </button>
          </form>
        </div>
      </div>
    </div>
  );
}
