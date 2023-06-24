---
layout: post
title: "Como criar um cronograma\ de estudo no Google Sheets\ sincronizado com o Google Calendar"
categories: [Generic, Javascript, Google, how-to]
bibliography: references.bib
---

<nav class="toc-fixed" markdown="1">
* TOC
{:toc}
</nav>

**Sincronizando Google Sheets e Google Calendar como ferramenta de estudo**

As ferramentas do G Suite são extremamentes poderosas e mostro como sincronizar o Google Sheets com o Google Calendar para um cronograma de estudo.

## O Appscript do Google Sheets

Assim é definido o <cite>[Appscript][1]</cite>

>  Apps Script is a cloud-based JavaScript platform that lets you integrate with and automate tasks across Google products.

A figura 1 ilustra como o Appscript é acessado

<p align="center">
  <img src="/images/sheets-calendar-sync/appscript_spreadsheet.png" />
</p>

Crie um novo script, como mostrado na figura 2

<p align="center">
  <img src="/images/sheets-calendar-sync/new_script.png" />
</p>

## Coleta do CalendarID

Colete o CalendarID do calendário específico no Google Calendar

## Template do Cronograma

O script utiliza-se dos parâmetros no cabeçalho da planilha, portanto alterá-la pode ocasionar a falha na sincronização do cronograma no sheets

## O script

O script foi dividido em 3 etapas + Criação do botão na Ui do Google Sheets

1. Setup das Classes to Calendar e Spreadsheets do Appscript
2. Tratar células vazias
3. Comparação dos eventos na planilha com os eventos já existentes no calendar
4. Criação do botão UI

~~~Javascript
function syncCalendarEvents() {
  /**
    * 1) Open the Event Calendar, spreadsheet, and get events in calendar.
  **/
  var spreadsheet = SpreadsheetApp.getActiveSheet();
  var calendarId = spreadsheet.getRange("C4").getValue();
  var calendar = CalendarApp.getCalendarById(calendarId);

  //get start day and last day of spreadsheet agenda
  var startDay = spreadsheet.getRange("C10").getValue();
  var endDay = spreadsheet.getRange("D10:D").getNextDataCell(SpreadsheetApp.Direction.DOWN).getValue();
  
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

## Referencias

[1]: https://developers.google.com/apps-script