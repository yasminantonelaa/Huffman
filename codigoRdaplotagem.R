dados <- read.csv("coparacoes.csv")

library(reshape2)
library(ggplot2)

dados_longos <- melt(dados, id.vars = "Numero",
                     variable.name = "TipoFila",
                     value.name = "Comparacoes")

# Criar o gráfico com pontos
ggplot(dados_longos, aes(x = Numero, y = Comparacoes, color = TipoFila)) +
  geom_line(size = 1) +
  geom_point(size = 2) +  # <-- Adiciona os pontos aqui
  labs(
    title = "Comparação de Número de Comparações",
    subtitle = "Fila de prioridade com e sem Heap",
    x = "Número buscado",
    y = "Número de comparações",
    color = "Tipo de Fila"
  ) +
  theme_minimal() +
  theme(
    plot.title = element_text(face = "bold", size = 14),
    plot.subtitle = element_text(size = 10),
    axis.title = element_text(size = 12),
    legend.position = "top"
  )