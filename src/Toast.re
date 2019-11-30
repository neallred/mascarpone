let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;
let str = ReasonReact.string;

type toastType = Success | Failure | Warning | Info;

let toastBoxClass = css(
  ~display="flex",
  (),
);

type toastDetails = {
  type_: toastType,
  message: string,
  time_of_appearance: float, // in ms, used for ordering toasts, highest first
};

let getToastDuration = type_ => {
  switch type_ {
    | Success => 15 * 1000
    | Failure => 120 * 1000
    | Warning => 60 * 1000
    | Info => 15 * 1000
  }
};

[@react.component]
let make = (
  ~toast: toastDetails,
  ~toastKey,
  ~clearToast,
) => {
  React.useEffect0(() => {
    Js.log3("toast effect!", toast, toastKey);
    let duration = getToastDuration(toast.type_);

    let timeout = Js.Global.setTimeout(() => {
      clearToast(toastKey);
    }, duration);
    Some(() => {
      Js.Global.clearTimeout(timeout);
    });
  });
  <div style={toastBoxClass}>
    <div></div>
    <div>{str(toast.message)}</div>
  </div>
};

