import React, { useState } from "react";
import axios from "axios";

export default function CoursePage() {
  const [courseName, setCourseName] = useState("");
  const [teacher, setTeacher] = useState("");
  const [student, setStudent] = useState("");
  const [courseId, setCourseId] = useState("");

  const createCourse = async () => {
    try {
      const res = await axios.post("http://localhost:18080/course/create", {
        name: courseName,
        teacher,
      });
      alert(res.data);
    } catch (err) {
      alert("Error: " + err.message);
    }
  };

  const enrollCourse = async () => {
    try {
      const res = await axios.post(`http://localhost:18080/course/${courseId}/enroll`, {
        student,
      });
      alert(res.data);
    } catch (err) {
      alert("Error: " + err.message);
    }
  };

  return (
    <div>
      <h2>课程管理</h2>
      <h3>教师创建课程</h3>
      <input placeholder="课程名" onChange={(e) => setCourseName(e.target.value)} />
      <input placeholder="教师用户名" onChange={(e) => setTeacher(e.target.value)} />
      <button onClick={createCourse}>创建课程</button>

      <h3>学生选课</h3>
      <input placeholder="课程ID" onChange={(e) => setCourseId(e.target.value)} />
      <input placeholder="学生用户名" onChange={(e) => setStudent(e.target.value)} />
      <button onClick={enrollCourse}>选课</button>
    </div>
  );
}
