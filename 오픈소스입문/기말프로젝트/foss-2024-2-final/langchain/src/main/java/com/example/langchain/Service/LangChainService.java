package com.example.langchain.Service;

import com.example.langchain.model.ChatHistory;
import com.example.langchain.repository.ChatHistoryRepository;
import org.springframework.stereotype.Service;
import org.springframework.web.reactive.function.client.WebClient;

import java.time.LocalDateTime;
import java.util.Map;

@Service
public class LangChainService {

    private final WebClient webClient;
    private final ChatHistoryRepository chatHistoryRepository;

    public LangChainService(WebClient.Builder webClientBuilder, ChatHistoryRepository chatHistoryRepository) {
        this.webClient = webClientBuilder.baseUrl("https://api.langchain.com").build();
        this.chatHistoryRepository = chatHistoryRepository;
    }

    public String getResponse(String userMessage) {
        // LangChain API 호출
        Map<String, String> requestBody = Map.of("message", userMessage);

        String botResponse = webClient.post()
                .uri("/v1/chat")
                .bodyValue(requestBody)
                .retrieve()
                .bodyToMono(String.class)
                .block();

        // 채팅 기록 저장
        ChatHistory chatHistory = new ChatHistory(null, userMessage, botResponse, LocalDateTime.now());
        chatHistoryRepository.save(chatHistory);

        return botResponse;
    }
}
