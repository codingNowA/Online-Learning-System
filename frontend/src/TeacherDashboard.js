import React, { useEffect, useState } from "react";

function TeacherDashboard({ username }) {
  const [courses, setCourses] = useState([]);
  const [assignments, setAssignments] = useState([]);

  useEffect(() => {
    // 获取老师任课课程
    fetch(`http://localhost:18080/teacher/${username}/courses`)
      .then(res => res.json())
      .then(data => {
        if (data) {
          setCourses(Object.values(data));
        } else {
          setCourses([]);
        }
      })
      .catch(err => {
        console.error("课程获取失败:", err);
        setCourses([]);
      });

    // 获取老师布置的作业
    fetch(`http://localhost:18080/teacher/${username}/assignments`)
      .then(res => res.json())
      .then(data => {
        if (data) {
          setAssignments(Object.values(data));
        } else {
          setAssignments([]);
        }
      })
      .catch(err => {
        console.error("作业获取失败:", err);
        setAssignments([]);
      });
  }, [username]);

  return (
    <div>
      <h2>教师主页 - {username}</h2>

      <h3>任课课程</h3>
      {courses.length === 0 ? (
        <p>暂无课程</p>
      ) : (
        <ul>
          {courses.map(c => (
            <li key={c.id}>{c.name}</li>
          ))}
        </ul>
      )}

      <h3>布置的作业</h3>
      {assignments.length === 0 ? (
        <p>暂无作业</p>
      ) : (
        <ul>
          {assignments.map(a => (
            <li key={a.id}>
              {a.title} | 截止: {a.due_date} <br />
              描述: {a.description}
            </li>
          ))}
        </ul>
      )}
    </div>
  );
}

export default TeacherDashboard;


