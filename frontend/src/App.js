import React, { useState } from "react";
import LoginPage from "./LoginPage";
import StudentDashboard from "./StudentDashboard";
import TeacherDashboard from "./TeacherDashboard";

function App() {
  const [role, setRole] = useState(null);
  const [username, setUsername] = useState("");

  const handleLogin = (role, username) => {
    setRole(role);
    setUsername(username);
  };

  if (!role) {
    return <LoginPage onLogin={handleLogin} />;
  }

  if (role === "student") {
    return <StudentDashboard username={username} />;
  }

  if (role === "teacher") {
    return <TeacherDashboard username={username} />;
  }

  return <div>未知角色</div>;
}

export default App;
