import React, { useEffect, useState } from "react";

function StudentDashboard({ username }) {
  const [courses, setCourses] = useState([]);
  const [submissions, setSubmissions] = useState([]);

  useEffect(() => {
    // 获取学生选课
    fetch(`http://localhost:18080/student/${username}/courses`)
      .then(res => res.json())
      .then(data => {
        if (data) {
          setCourses(Object.values(data));
        } else {
          setCourses([]); // 防止 null 报错
        }
      })
      .catch(err => {
        console.error("课程获取失败:", err);
        setCourses([]);
      });

    // 获取学生提交的作业
    fetch(`http://localhost:18080/student/${username}/submissions`)
      .then(res => res.json())
      .then(data => {
        if (data) {
          setSubmissions(Object.values(data));
        } else {
          setSubmissions([]);
        }
      })
      .catch(err => {
        console.error("作业获取失败:", err);
        setSubmissions([]);
      });
  }, [username]);

  return (
    <div>
      <h2>学生主页 - {username}</h2>

      <h3>已选课程</h3>
      {courses.length === 0 ? (
        <p>暂无课程</p>
      ) : (
        <ul>
          {courses.map(c => (
            <li key={c.id}>
              {c.name}（教师: {c.teacher}）
            </li>
          ))}
        </ul>
      )}

      <h3>提交的作业</h3>
      {submissions.length === 0 ? (
        <p>暂无作业提交</p>
      ) : (
        <ul>
          {submissions.map(s => (
            <li key={s.id}>
              作业 {s.assignment_id}: {s.content} | 成绩: {s.grade || "未评分"} | 评语: {s.comments || "暂无"}
            </li>
          ))}
        </ul>
      )}
    </div>
  );
}

export default StudentDashboard;

