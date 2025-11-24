import React, { useEffect, useState } from "react";

function StudentDashboard({ username }) {
  const [courses, setCourses] = useState([]);
  const [availableCourses, setAvailableCourses] = useState([]);
  const [submissions, setSubmissions] = useState([]);
  const [pendingAssignments, setPendingAssignments] = useState([]);
  const [assignmentId, setAssignmentId] = useState("");
  const [title, setTitle] = useState("");
  const [content, setContent] = useState("");

  // 加载已选课程、可选课程、已提交作业、未完成作业
  useEffect(() => {
    fetch(`http://localhost:18080/student/${username}/courses`)
      .then(res => res.json())
      .then(data => setCourses(Array.isArray(data) ? data : Object.values(data || {})));

    fetch(`http://localhost:18080/student/${username}/available_courses`)
      .then(res => res.json())
      .then(data => setAvailableCourses(Array.isArray(data) ? data : Object.values(data || {})));

    fetch(`http://localhost:18080/student/${username}/submissions`)
      .then(res => res.json())
      .then(data => setSubmissions(Array.isArray(data) ? data : Object.values(data || {})));

    fetch(`http://localhost:18080/student/${username}/pending_assignments`)
      .then(res => res.json())
      .then(data => setPendingAssignments(Array.isArray(data) ? data : Object.values(data || {})));
  }, [username]);

  const handleEnroll = (courseId) => {
    fetch(`http://localhost:18080/course/${courseId}/enroll`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ student: username })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        // 刷新已选课程、可选课程、未完成作业
        return Promise.all([
          fetch(`http://localhost:18080/student/${username}/courses`).then(res => res.json()),
          fetch(`http://localhost:18080/student/${username}/available_courses`).then(res => res.json()),
          fetch(`http://localhost:18080/student/${username}/pending_assignments`).then(res => res.json())
        ]);
      })
      .then(([enrolled, available, pending]) => {
        setCourses(Array.isArray(enrolled) ? enrolled : Object.values(enrolled || {}));
        setAvailableCourses(Array.isArray(available) ? available : Object.values(available || {}));
        setPendingAssignments(Array.isArray(pending) ? pending : Object.values(pending || {}));
      })
      .catch(err => alert("选课失败: " + err));
  };


  // 提交作业
  const handleSubmit = () => {
    fetch(`http://localhost:18080/assignment/${username}/submit`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        assignment_id: parseInt(assignmentId),
        title,
        content
      })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        // 刷新已提交和未完成作业列表
        return Promise.all([
          fetch(`http://localhost:18080/student/${username}/submissions`).then(res => res.json()),
          fetch(`http://localhost:18080/student/${username}/pending_assignments`).then(res => res.json())
        ]);
      })
      .then(([subs, pending]) => {
        setSubmissions(Array.isArray(subs) ? subs : Object.values(subs || {}));
        setPendingAssignments(Array.isArray(pending) ? pending : Object.values(pending || {}));
      })
      .catch(err => alert("提交失败: " + err));
  };

  return (
    <div>
      <h2>学生主页 - {username}</h2>

      <h3>已选课程</h3>
      <ul>
        {courses.length === 0 ? (
          <li>暂无已选课程</li>
        ) : (
          courses.map(c => (
            <li key={c.id}>{c.name}（教师: {c.teacher}）</li>
          ))
        )}
      </ul>

      <h3>可选课程</h3>
      <ul>
        {availableCourses.length === 0 ? (
          <li>暂无可选课程</li>
        ) : (
          availableCourses.map(c => (
            <li key={c.id}>
              {c.name}（教师: {c.teacher}）
              <button onClick={() => handleEnroll(c.id)}>选课</button>
            </li>
          ))
        )}
      </ul>

      <h3>已提交的作业</h3>
      <ul>
        {submissions.length === 0 ? (
          <li>暂无已提交作业</li>
        ) : (
          submissions.map(s => (
            <li key={s.id}>
              作业 {s.assignment_id} - {s.title}: {s.content} | 成绩: {s.grade || "未评分"} | 评语: {s.comments || "暂无"}
            </li>
          ))
        )}
      </ul>

      <h3>未完成的作业</h3>
      <ul>
        {pendingAssignments.length === 0 ? (
          <li>暂无未完成作业</li>
        ) : (
          pendingAssignments.map(a => (
            <li key={a.id}>
              作业 {a.id} - {a.title} | 截止: {a.due_date} | 描述: {a.description}
            </li>
          ))
        )}
      </ul>

      <h3>提交新作业</h3>
      <div>
        <input
          type="text"
          placeholder="作业ID"
          value={assignmentId}
          onChange={e => setAssignmentId(e.target.value)}
        />
        <input
          type="text"
          placeholder="作业标题"
          value={title}
          onChange={e => setTitle(e.target.value)}
        />
        <textarea
          placeholder="作业内容"
          value={content}
          onChange={e => setContent(e.target.value)}
        />
        <button onClick={handleSubmit}>提交</button>
      </div>
    </div>
  );
}

export default StudentDashboard;


