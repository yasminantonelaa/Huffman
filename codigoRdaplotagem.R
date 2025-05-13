# Carrega pacotes
library(reshape2)
library(ggplot2)

# Lê os dados
dados <- read.csv("coparacoes.csv")

# Transforma para formato longo
dados_longos <- melt(dados, id.vars = "Numero",
                     variable.name = "TipoFila",
                     value.name = "Comparacoes")

# Separa os dados por tipo
dados_heap     <- subset(dados_longos, TipoFila == "FilaComHeap")
dados_sem_heap <- subset(dados_longos, TipoFila == "FilaSemHeap")

# Gráfico para Fila com Heap
grafico_heap <- ggplot(dados_heap, aes(x = Numero, y = Comparacoes)) +
  geom_line(color = "blue", size = 1) +
  geom_point(color = "blue", size = 2) +
  labs(
    title = "Comparações de Busca - Fila com Heap",
    x = "Número buscado",
    y = "Número de comparações"
  ) +
  theme_minimal()

# Gráfico para Fila sem Heap (lista)
grafico_lista <- ggplot(dados_sem_heap, aes(x = Numero, y = Comparacoes)) +
  geom_line(color = "red", size = 1) +
  geom_point(color = "red", size = 2) +
  labs(
    title = "Comparações de Busca - Fila sem Heap",
    x = "Número buscado",
    y = "Número de comparações"
  ) +
  theme_minimal()

# Salvar os gráficos
ggsave("grafico_heap.png", grafico_heap, width = 8, height = 5, bg = "white")
ggsave("grafico_lista.png", grafico_lista, width = 8, height = 5, bg = "white")
