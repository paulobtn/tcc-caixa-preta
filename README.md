# (TCC) Caixa Preta para carros: proposta para calibração, coleta e fusão de dados

Este é meu trabalho de conclusão do curso de Engenharia da Computação pela
Universidade de Brasília. Orientado por Ricardo Zelenovsky.

Consiste num sistema embarcado que coleta dados de movimentações de um veículo,
como uma caixa preta, para caracterizar acidentes.

Minha maior contribuição foi ter adicionado o magnetômetro, além dos sensores
de giroscópio e acelerômetro que existiam em propostas anteriores. Em meu artigo
mostrei que o magnetômetro é necessário para estimar corretamente a movimentação 
em três dimensões. Também criei rotinas de self-test e calibração dos sensores.

Para mais informações, consulte monografia-paulo.pdf

** Organização **

```
* CXP/                 -   arquivos do código embarcado
* MATLAB/              -   Scripts para calibrar, coletar, filtrar e simular dados
* monografia-paulo.pdf -   Monografia
```

** Bibliotecas externas **

* MATLAB/filtros/MadgwickAHRS 
* MATLAB/filtros/quaternion_library são bibliotecas

Desenvolvidas por [Sebastian Madgwick](https://x-io.co.uk/open-source-imu-and-ahrs-algorithms/).
A primeira é para aplicar o [filtro de Madgwick](https://www.x-io.co.uk/res/doc/madgwick_internal_report.pdf),
a segunda é para manipular Quaternions no matlab.

