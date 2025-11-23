import React, { useState } from "react";
import axios from "axios";

export default function AssignmentPage() {
  const [courseId, setCourseId] = useState("");
  const [teacher, setTeacher] = useState("");
  const [title, setTitle] = useState("");
  const [description, setDescription] = useState("");
  const [dueDate, setDueDate] = useState("");
  const [student, setStudent] = useState("");
  const [content, setContent] = useState("");
  const [submissionId, setSubmissionId] = useState("");
  const [grade, setGrade] = useState("");
  const [comments, setComments] = useState("");

  const createAssignment = async () => {
    await axios.post("http://localhost:18080/assignment/create", {
      course_id: parseInt(courseId),
      teacher,
      title,
      description,
      due_date: dueDate,
    });
    alert("作业创建成功");
  };

  const submitAssignment = async () => {
    await axios.post(`http://localhost:18080/assignment/${courseId}/submit`, {
      student,
      content,
    });
    alert("作业提交成功");
  };

  const gradeAssignment = async () => {
    await axios.post(`http://localhost:18080/assignment/${submissionId}/grade`, {
      teacher,
      grade,
      comments,
    });
    alert("评分成功");
  };

  return (
    <div>
      <h2>作业管理</h2>
      <h3>教师布置作业</h3>
      <input placeholder="课程ID" onChange={(e) => setCourseId(e.target.value)} />
      <input placeholder="教师用户名" onChange={(e) => setTeacher(e.target.value)} />
      <input placeholder="标题" onChange={(e) => setTitle(e.target.value)} />
      <input placeholder="描述" onChange={(e) => setDescription(e.target.value)} />
      <input placeholder="截止日期" onChange={(e) => setDueDate(e.target.value)} />
      <button onClick={createAssignment}>布置作业</button>

      <h3>学生提交作业</h3>
      <input placeholder="学生用户名" onChange={(e) => setStudent(e.target.value)} />
      <textarea placeholder="作业内容" onChange={(e) => setContent(e.target.value)} />
      <button onClick={submitAssignment}>提交作业</button>

      <h3>教师评分</h3>
      <input placeholder="提交ID" onChange={(e) => setSubmissionId(e.target.value)} />
      <input placeholder="教师用户名" onChange={(e) => setTeacher(e.target.value)} />
      <input placeholder="成绩" onChange={(e) => setGrade(e.target.value)} />
      <input placeholder="评语" onChange={(e) => setComments(e.target.value)} />
      <button onClick={gradeAssignment}>评分</button>
    </div>
  );
}
