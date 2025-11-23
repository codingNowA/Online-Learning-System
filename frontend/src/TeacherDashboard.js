import React from "react";

function TeacherDashboard({ username }) {
  return (
    <div>
      <h2>教师主页 - {username}</h2>
      <p>这里以后展示老师的课程和作业。</p>
    </div>
  );
}

export default TeacherDashboard;

