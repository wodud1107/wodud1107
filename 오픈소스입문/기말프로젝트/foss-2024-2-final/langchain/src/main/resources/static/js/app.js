function sendMessage() {
    const userMessage = document.getElementById('user-message').value;

    if (userMessage.trim() === "") return;

    // Show the user's message in the chat
    const messagesDiv = document.getElementById('messages');
    messagesDiv.innerHTML += `<div><strong>You:</strong> ${userMessage}</div>`;

    // Clear the input field
    document.getElementById('user-message').value = "";

    // Call the backend API to get the response from LangChain
    fetch('http://localhost:8080/api/langchain/chat', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ message: userMessage })
    })
    .then(response => response.json())
    .then(data => {
        const botMessage = data.response;

        // Show the bot's response in the chat
        messagesDiv.innerHTML += `<div><strong>Bot:</strong> ${botMessage}</div>`;

        // Scroll to the bottom to show the latest messages
        messagesDiv.scrollTop = messagesDiv.scrollHeight;
    })
    .catch(error => {
        console.error('Error:', error);
        alert('There was an error processing your message.');
    });
}
