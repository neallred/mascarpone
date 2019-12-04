let css = ReactDOMRe.Style.make;
let str = ReasonReact.string;
let _0 = ReasonReact.null;
let multi = (x) => x |> Array.of_list |> ReasonReact.array;

let bodyClass = css(
  ~maxWidth="800px",
  ~width="calc(100vw - 20px)",
  ~margin="0 auto",
  ~position="relative",
  ~height="100vh",
  ~overflowX="hidden",
  ()
)

let headerClass = css(
  ~textAlign="center",
  ~padding="0.1em",
  ()
);

let requestHeaders = Fetch.HeadersInit.make({"Content-Type": "application/json"});

[@bs.val] external staticDnsRecordsJs: Js.Json.t = "pageLoadBlacklist";
let reshapeEntries = (json: Js.Json.t) => {
  json
    |> Json.Decode.dict(DnsRow.decodePartialDnsRecord)
    |> Js.Dict.entries
    |> Array.to_list
    |> List.map(((domain: Js.Dict.key, other: DnsRow.partialDnsRecord)) => {
        let myDnsRecord: DnsRow.dnsRecord = {
          domain: domain,
          redirect: other.redirect,
          updated: other.updated,
        };

        myDnsRecord
      })
}

let staticDnsRecords: list(DnsRow.dnsRecord) = staticDnsRecordsJs |> reshapeEntries 

type viewingMode = 
  | Viewing
  | ConfirmingDelete(string)
  | EditingRecord(string)

type serverInteractionStatus =
  | Bootstrapping
  | Idle
  | SendingUpdate(string)
  | SendingNew(string)
  | SendingDelete(string)
  | FatalError(string)

type state = {
  viewingMode: viewingMode,
  serverStatus: serverInteractionStatus,
  dnsRecords: option(list(DnsRow.dnsRecord)),
  isAddingNew: bool,
  searchTerm: string,
  toasts: ToastMaster.SMap.t(Toast.toastDetails),
};

type action = 
  | CancelDelete
  | ConfirmDelete(string)
  | ConfirmInsert(NewRecord.newItem)
  | ConfirmUpdate(EditDnsRow.editItem)
  | SetAddingNew(bool)
  | StartDelete(string)
  | StartEdit(string)
  | CancelEdit
  | UpdateRecord(string, string)
  | ReceiveRecords(list(DnsRow.dnsRecord))
  | SetSearchTerm(string)
  | ClearToast(string)

let initialState = {
  viewingMode: Viewing,
  serverStatus: Bootstrapping,
  dnsRecords: None,
  isAddingNew: false,
  searchTerm: "",
  // toasts: ToastMaster.emptyToasts,
  toasts: ToastMaster.errorToasts,
};

let reducer = (state, action) => {
  switch (action) {
    | CancelDelete => {
        ...state,
        viewingMode: Viewing,
      }
    | ConfirmDelete(domain) => {
      ...state,
      serverStatus: SendingDelete(domain),
      viewingMode: Viewing,
    }
    | ConfirmInsert(theNewItem) => {
      ...state,
      serverStatus: SendingNew("Adding " ++ theNewItem.domain),
      viewingMode: Viewing
    }
    | ConfirmUpdate(editedItem) => {
      ...state,
      serverStatus: SendingUpdate("Updating " ++ editedItem.domain),
      viewingMode: Viewing,
    }
    | SetAddingNew(isAddingNew) => {
        ...state,
        isAddingNew
      }
    | StartDelete(domain) => {
        ...state,
        viewingMode: ConfirmingDelete(domain)
      }
    | StartEdit(domain) => {
        ...state,
        viewingMode: EditingRecord(domain)
      }
    | CancelEdit => {
        ...state,
        viewingMode: Viewing,
      }
    | UpdateRecord(domain, _) => {
      ...state,
      viewingMode: Viewing,
      serverStatus: SendingUpdate(domain),
    }
    | ReceiveRecords(records) => {
      ...state,
      viewingMode: Viewing,
      serverStatus: Idle,
      isAddingNew: false,
      dnsRecords: Some(records),
    }
    | SetSearchTerm(searchTerm) => {
      ...state,
      searchTerm
    }
    | ClearToast(toastKey: string) => {
      ...state,
      toasts: ToastMaster.SMap.remove(toastKey, state.toasts),
    }
  }
};

let staticStylesheet = <style dangerouslySetInnerHTML={{"__html": Styles.stylesheet}} />;

let document = Webapi.Dom.Document.asEventTarget(Webapi.Dom.document);

[@bs.scope "document"] [@bs.val] external body: Dom.element = "body";

[@bs.scope "document"] [@bs.val]
external addKeybordEventListener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "addEventListener";

[@bs.scope "document"] [@bs.val]
external removeKeybordEventListener:
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "removeEventListener";


// looks like this is a great resource for integrating with JS,
// including global keyboard events listeners
// https://dev.to/seif_ghezala/reasonml-for-production-react-apps-part-3-3368
[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(reducer, initialState);
  let {
    viewingMode,
    isAddingNew,
    searchTerm,
    toasts,
  } = state;

  
  let keyDownListener = (e) => {
    let key = ReactEvent.Keyboard.keyCode(e);
    Js.log2("Key down", key);
    if (key === 27) {
      let _ = switch viewingMode {
        | Viewing => {
          ();
        }
        | ConfirmingDelete(_) => {
          dispatch(CancelDelete);
        }
        | EditingRecord(_) => {
          dispatch(CancelEdit);
        }
      }
      if (isAddingNew) {
        dispatch(SetAddingNew(false));
      };
    }
  };

  let effect = () => {
    addKeybordEventListener("keydown", keyDownListener)
    Some(() => removeKeybordEventListener("keydown", keyDownListener))
  };
  React.useEffect3(effect, (keyDownListener, viewingMode, isAddingNew))

  React.useEffect0(() => {
    dispatch(ReceiveRecords(staticDnsRecords));
    None
  });

  let {
    viewingMode,
    serverStatus,
    dnsRecords,
    isAddingNew,
  } = state;

  let canStartRowAction = serverStatus === Idle && viewingMode == Viewing;
  let recordsList = switch dnsRecords {
    | None => []
    | Some(records) => records
  };
  let filteredRecordsList = List.filter(
    (x: DnsRow.dnsRecord) => (
      Js.String.includes(searchTerm, x.domain) ||
      Js.String.includes(searchTerm, x.redirect) ||
      viewingMode == EditingRecord(x.domain) ||
      viewingMode == ConfirmingDelete(x.domain)
    ),
    recordsList
  );

  let requestDelete = React.useCallback1((domain: string) => {
    Js.log("user requested delete of " ++ domain);
    dispatch(StartDelete(domain))
  }, [|dispatch|]);

  let confirmDelete = React.useCallback1((domain: string) => {
    dispatch(ConfirmDelete(domain));

    let payload = Js.Dict.empty();
    Js.Dict.set(payload, "domain", Js.Json.string(domain));
    let _ = Js.Promise.(
      Fetch.fetchWithInit(
        "/",
        Fetch.RequestInit.make(
          ~method_=Delete,
          ~body=Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
          ~headers=requestHeaders,
          ()
        ),
      )
      |> then_(Fetch.Response.json)
      |> then_(json => {

        dispatch(ReceiveRecords(json |> reshapeEntries));
      json |> resolve
      })
    );
    ()
  }, [|dispatch|]);

  let confirmInsert = React.useCallback1((newRecord: NewRecord.newItem) => {
    dispatch(ConfirmInsert(newRecord));
    let payload = Js.Dict.empty();
    Js.Dict.set(payload, "domain", Js.Json.string(newRecord.domain));
    Js.Dict.set(payload, "redirect", Js.Json.string(newRecord.redirect));
    let _ = Js.Promise.(
      Fetch.fetchWithInit(
        "/",
        Fetch.RequestInit.make(
          ~method_=Post,
          ~body=Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
          ~headers=requestHeaders,
          ()
        ),
      )
      |> then_(Fetch.Response.json)
      |> then_(json => {
        dispatch(ReceiveRecords(json |> reshapeEntries));
      json |> resolve
      })
    );
    ()
  }, [|dispatch|]);

  let cancelDelete = React.useCallback1(() => {
    dispatch(CancelDelete);
  }, [|dispatch|]);

  let requestEdit = React.useCallback1((domain: string) => {
    Js.log("user requested edit of " ++ domain);
    dispatch(StartEdit(domain))
  }, [|dispatch|]);

  let cancelEdit = React.useCallback1(() => {
    dispatch(CancelEdit);
  }, [|dispatch|]);

  let confirmEdit = React.useCallback1((item: EditDnsRow.editItem) => {
    dispatch(ConfirmUpdate(item));
    let payload = Js.Dict.empty();
    Js.Dict.set(payload, "domain", Js.Json.string(item.domain));
    Js.Dict.set(payload, "newDomain", Js.Json.string(item.newDomain));
    Js.Dict.set(payload, "redirect", Js.Json.string(item.redirect));
    let _ = Js.Promise.(
      Fetch.fetchWithInit(
        "/",
        Fetch.RequestInit.make(
          ~method_=Put,
          ~body=Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
          ~headers=requestHeaders,
          ()
        ),
      )
      |> then_(Fetch.Response.json)
      |> then_(json => {
        dispatch(ReceiveRecords(json |> reshapeEntries));
      json |> resolve
      })
    );
    ()
  }, [|dispatch|]);

  let dnsRows = List.map(
    (x: DnsRow.dnsRecord) => {
      viewingMode == EditingRecord(x.domain)
        ? <EditDnsRow
          key={x.domain}
          dnsRecord={x}
          cancelEdit 
          confirmEdit
        />
        : <DnsRow
          key={x.domain}
          canShowControls={canStartRowAction}
          dnsRecord={x}
          isDeleting={viewingMode == ConfirmingDelete(x.domain)}
          requestDelete
          requestEdit
          cancelDelete 
          confirmDelete
        />
    },
    filteredRecordsList,
  );

  <div>
  {staticStylesheet}
    <div style={bodyClass}>
      <header style={headerClass}>
        <Title
          visible={List.length(filteredRecordsList)}
          total={List.length(recordsList)}
        />
        <ListFilterer
          isAddingNew={isAddingNew}
          searchTerm={searchTerm}
          updateSearchTerm={x => dispatch(SetSearchTerm(x));}
        />
      </header>
      <div>
      {multi(dnsRows)}
      </div>
      <OpenNewRecord
        openNewRecord={(_) => dispatch(SetAddingNew(true))}
        isOpen={isAddingNew}
      />
    </div>
    <ToastMaster
      toasts
      clearToast={toastKey => dispatch(ClearToast(toastKey))} />
    <NewRecord
      closeAddNew={(_) => dispatch(SetAddingNew(false))}
      confirmAddNew={confirmInsert}
      isOpen={isAddingNew}
    />
  </div>
};


let appShape = {|
<App>
  <Title recordsShown totalRecords />
  <ListFilterer />
  <DnsTable records />
  <OpenNewRecord />
  <NewRecord />
</App>

where 
  <DnsTable records /> = 
  <div>
    <DnsHeader />
    multi(<DnsRecord />)
  </div>
|};
