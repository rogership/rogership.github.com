---
layout: post
title: "Sync Google Sheets com o Google Calendar"
categories: [Produtividade, Javascript, G Suite]
---

<nav class="toc-fixed" markdown="1">
* TOC
{:toc}
</nav>

Video
<iframe width="420" height="315" src="/images/sheets-calendar-sync/sync-preview.mp4" frameborder="0" allowfullscreen></iframe>

Gif
{:refdef: style="text-align: center;"}
![My Image](/images/sheets-calendar-sync/sync-preview.gif)
{: refdef}


As ferramentas do G Suite são extremamentes poderosas e neste artigo mostro como sincronizar o Google Sheets com o Google Calendar para um cronograma de estudo.


## O Appscript do Google Sheets e setups

Assim é definido o **Appscript**:

>  Apps Script is a cloud-based JavaScript platform that lets you integrate with and automate tasks across Google products.

Este artigo trata-se de um código para sincronizar cronograma de estudo no Google Sheets com o Google Calendar.

O script permite gerenciar todas as agendas através da planilha,

1. Criar novas agendas (eventos)
2. Atualizar agendas existentes
3. Deletar eventos

Apenas a planilha é necessária para gerar, atualizar e deletar eventos.


### Coleta do CalendarID

Para sincronizar o Google Sheets com o Google Calendar será necessário coletar o CalendarID do calendário

<p align="center">
  <img src="/images/sheets-calendar-sync/calendar_settings.png" width="640" height="480"/>
</p>

### Template do Cronograma

O template já herda o script e toda a estrutura da planilha para uso, portanto, fiquem a vontade para copiar o [Template Cronograma](https://docs.google.com/spreadsheets/d/18yDeBRuJG0KqTidovBiFiWT06EAvJa2YV39_jBgzroI/edit?usp=sharing) 

Insira o calendarId na célula específica, será necessário autorizar as permissões de uso do Google sheets para o Google Calendar, este processo é feito ao executar o script pela primeira vez.

## Execução do script

É possível criar uma nova guia no menu de ferramentas, assim para executar o script basta clicar em *Sincronizar agenda* no menu.

<p align="center">
  <img src="/images/sheets-calendar-sync/sheets_sync_ui_640.png"/>
</p>

## O script

A planilha manipula todas as agendas do calendário sem necessidade de interação manual pelo Google Calendar.

As ações do script são descritas a seguir.

**Ações:**

1. Criar Agenda
2. Atualizar hora de início, hora fim e descrição de um evento
3. Deletar eventos inexistentes na planilha

O script foi dividido em 3 etapas + Criação do botão na Ui do Google Sheets

1. Setup de classes do google calendar e spreadsheet
2. Tratar células vazias
3. Laço onde acontece as ações
4. Criação do botão UI

Existe uma excelente documentação das classes existentes para uso em integração com o G Suite

Utiliza-se dos serviços ***Calendar*** e do ***Sheets***.

As referências podem ser consultadas em https://developers.google.com/apps-script/reference.

### Classes CalendarApp e SpreadsheetApp

Cria-se os objeteos de cada serviço, Calendar e Sheets, trata-se os valores necessários e principalmente criamos um dicionário chamado **eventMap** com eventos já existentes no calendário com estrutura


> {key: value}
> { "Título do Evento" : Objeto do evento}


~~~Javascript
  /**
    * 1) Open the Event Calendar, spreadsheet, and get events in calendar.
  **/
  var spreadsheet = SpreadsheetApp.getActiveSheet();
  var calendarId = spreadsheet.getRange("C4").getValue();
  var calendar = CalendarApp.getCalendarById(calendarId);

  //get start day and last day of spreadsheet agenda
  var startDay = spreadsheet.getRange("C10").getValue();
  var values = spreadsheet.getRange("D10:D").getValues();
  var endDay = values.filter(x => x != "").slice(-1)[0]
  
  //console.log("Start Day: " + startDay);
  //console.log("End Day: " + endDay);

  var events = calendar.getEvents(new Date(startDay), new Date(endDay));
  var eventMap = {};

  // Create a map of existing calendar events
  for (var i = 0; i < events.length; i++) {
    var event = events[i];
    var eventTitle = event.getTitle();
    eventMap[eventTitle] = event;

    Logger.log('Event on Calendar: ' + eventMap[eventTitle].getTitle());
  }
~~~

### Handle de células vazias

A classe Range coleta todos os valores em um range de células de uma planilha, inclusive valores vazios, portanto, é necessário tratar os valores vazios.

Esta etapa do script atribui a variável **lastRow** a última linha em uso da coluna *B10*, coluna dos tópicos de estudo.

A variável **lastColumn** recebe a última coluna em uso.

Assim trata-se todos os valores existentes do tópico, hora de início e hora de fim de estudo. 

~~~Javascript
  /**
    * 2) Handle sheets and non-empty cells
  **/
  var rangeRow = spreadsheet.getRange("B10");
  var lastRow = rangeRow.getNextDataCell(SpreadsheetApp.Direction.DOWN).getA1Notation();
  //gest last Column in A1Notation
  var rangeColumn = spreadsheet.getRange(lastRow);
  var lastColumn = rangeColumn.getNextDataCell(SpreadsheetApp.Direction.NEXT).getA1Notation();
  
  var sheetRange = spreadsheet.getRange("B10" + ":" + lastColumn);
  var studyDays = sheetRange.getValues();
  var size = sheetRange.getHeight();

  //console.log("Sheet height:" + size)
  //console.log("Last Row: " + lastRow)
  //console.log("Last Column: " + lastColumn)  
~~~

### O laço que tudo acontece

Este laço, trata-se eventos a serem criados, atualização da hora de início e hora final de eventos com mesmo nome ao tópico da planilha e apaga eventos que não estão na planilha.

Iteramos sobre toda a planilha e comparamos os tópicos com os títulos de eventos já existentes, a partir desse etapa, toma-se ação se cria o evento, atualiza ou deleta.

~~~Javascript
  /**
   * 3) For loop comparing calendar event titles and spreadsheet event titles
  **/

  for(x = 0; x < size; x++){

    //shift receives spreadsheet row values as list
    var shift = studyDays[x];
    //Logger .log('Var shift: ' + shift)

    var subject = shift[0];
    var startTime = shift[1];
    var endTime = shift[2];
    var descri = shift[3];

    //Logger.log('Row Index: ' + x + ' Sheet Subject: ' + subject);
    //Logger.log('Start Time: ' + startTime);
    //Logger.log('End Time: ' + endTime);
    //Logger.log('Description:' + descri)

    var event = eventMap[subject];

    //create or update event
    if (!event) {

      console.log("event not in calendar: " + subject);
      event = calendar.createEvent(subject, new Date(startTime), new Date(endTime));
      eventMap[subject] = event;

    }else{

      //event.setTitle(subject);
      event.setTime(new Date(startTime), new Date(endTime));

    }
    //set event description
    event.setDescription(descri)

  }

  for (var eventTitle in eventMap) {
    if (!spreadsheet.getRange("B10:B").getValues().flat().includes(eventTitle)) {
      eventMap[eventTitle].deleteEvent();
    }
  
  }
~~~

### A Ui

Para transformar o script em uma ação dem UI, cria-se um botão no layout do Google Sheets

~~~javascript
/**
 * 
 * 4) Make it easy to use.
 * UI GUI
 */

function onOpen() {
  var ui = SpreadsheetApp.getUi();
  ui.createMenu("Sync to Calendar")
    .addItem("Sincronizar a agenda", "syncCalendarEvents")
    .addToUi();
}
~~~

## Referências

https://developers.google.com/apps-script

https://cloud.google.com/blog/products/g-suite/g-suite-pro-tip-how-to-automatically-add-a-schedule-from-google-sheets-into-calendar

https://developers.google.com/apps-script