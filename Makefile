# CC = g++
# TARGET_DIR = target/
# SOURCE_DIR = src/

# TARGETA = client_exe
# TARGETB = server_exe
# SRCSA = client_main.cpp client.cpp utils.cpp
# SRCSB = server_main.cpp server.cpp utils.cpp
# OBJSA = $(patsubst %.cpp,$(TARGET_DIR)%.o,$(SRCSA))
# OBJSB = $(patsubst %.cpp,$(TARGET_DIR)%.o,$(SRCSB))

# $(TARGET_DIR)$(TARGETA): $(OBJSA)
# 	$(CC) -o $@ $^

# $(TARGET_DIR)%.o: $(SOURCE_DIR)%.cpp
# 	$(CC) -o $@ $<


# # $(TARGET_DIR)$(TARGETB): $(OBJSB)
# # 	$(CC) -o $@ $^ -lpthread

# # $(TARGET_DIR)%.o: $(SOURCE_DIR)%.c
# # 	$(CC) -c -o $@ $< -lpthread


# $(TARGET_DIR)$(TARGETB): $(OBJSB)
# 	$(CC) -o $@ $^

# $(TARGET_DIR)%.o: $(SOURCE_DIR)%.c
# 	$(CC) -c -o $@ $<

# clean:
# 	$(RM) $(TARGET_DIR)*.o $(TARGET_DIR)$(TARGETA) $(TARGET_DIR)$(TARGETB)