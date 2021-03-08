class Position extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
  }
  
  componentDidMount() {

  }
  
  render() {
    var pos = this.props.data,
        isShowDefault = Object.keys(pos).length==0 ? true : false
    
    var rows = []
    for(var [k, v] of Object.entries(pos)) {
      rows.push(
        <tr key={'pos_'+k}>
          <td>{k}</td>
          <td>{v}</td>
        </tr>
      )
    }
    
    return (
<div className="row">
<div className="col-12 col-sm-12">
<table className="table table-striped table-position">
<thead>
  <tr><th colSpan="2">仓位</th></tr>
</thead>
<tbody>
  <tr>
  <td width="50">证</td>
  <td width="50">量</td>
  </tr>
  {isShowDefault &&
  <tr>
  <td>-</td>
  <td>-</td>
  </tr>}
  {rows}
</tbody>
</table>
</div>
</div>

    )
  }
}
