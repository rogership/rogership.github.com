---
layout: post
title: "Como criar um cronograma\ de estudo no Google Sheets\ sincronizado com o Google Calendar"
categories: [Produtividade, Javascript, G Suite, how-to]
---

<nav class="toc-fixed" markdown="1">
* TOC
{:toc}
</nav>

**Sincronizando Google Sheets e Google Calendar como ferramenta de estudo**

![sheets-to-calendar-sync](./../images/sheets-calendar-sync/example2.gif)

As ferramentas do G Suite são extremamentes poderosas e este artigo mostra como sincronizar o Google Sheets com o Google Calendar para um cronograma de estudo.

## O Appscript do Google Sheets e setups

Segundo a google, <cite>[Appscript][1]</cite>:

>  Apps Script is a cloud-based JavaScript platform that lets you integrate with and automate tasks across Google products.

Assim, usaremos o Google Sheets como agenda de cronograma de estudo sincronizado com o Google Calendar.

**A planilha criar, atualiza e apaga as agendas diretamente no calendar.
**

Vamos precisar do CalendarID, portanto, copiamos o CalendarID nas configurações do Google Calendar e da planilha de cronograma.

![](gif do calendarID)

### Template do Cronograma

Fiquem a vontade para copiar o [Template Cronograma](https://docs.google.com/spreadsheets/d/18yDeBRuJG0KqTidovBiFiWT06EAvJa2YV39_jBgzroI/edit?usp=sharing)

O template já possui o script e a Ui, *Sync to Calendar* por padrão, pronto para uso.

Cole o CalendarID na na célula específica e a planilha estará pronta para uso.

<p align="center">
  <img src="/images/sheets-calendar-sync/appscript_spreadsheet.png" />
</p>
 

<p align="center">
  <img src="/images/sheets-calendar-sync/new_script.png" />
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

### Classes CalendarApp e SpreadsheetApp

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

~~~javascript
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

~~~javascript
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


### O laço que tudo acontece


### A Ui

## Referencias

[1]: https://developers.google.com/apps-script