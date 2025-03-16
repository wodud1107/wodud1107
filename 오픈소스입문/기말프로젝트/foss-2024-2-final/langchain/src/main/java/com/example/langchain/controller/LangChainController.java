package com.example.langchain.controller;

import com.example.langchain.model.ChatRequest;
import com.example.langchain.Service.LangChainService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/langchain")
public class LangChainController {

    private final LangChainService langChainService;

    public LangChainController(LangChainService langChainService) {
        this.langChainService = langChainService;
    }

    @PostMapping("/chat")
    public ResponseEntity<String> handleChat(@RequestBody ChatRequest chatRequest) {
        String response = langChainService.getResponse(chatRequest.getMessage());
        return ResponseEntity.ok(response);
    }
}
