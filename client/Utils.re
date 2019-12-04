let format_date = (timestamp) => {
  let timestamp_ms = timestamp *. 1000.0;
  let date = Js.Date.fromFloat(timestamp_ms);
  let date_str = Js.Date.toLocaleDateString(date);
  let time_str = Js.Date.toLocaleTimeString(date);
  date_str ++ " " ++ time_str
};

